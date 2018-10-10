#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include "log.h"

#define THREAD_NAME "log"

typedef struct anna_log_buffer anna_log_buffer;
struct anna_log_buffer {
	size_t allocated;
	anna_log_buffer *next;
	/* str is grown by allocating beyond the struct */
	char str[1];
};

#define INITIAL_BUFSZ		2000

static bool anna_log_console = true;
static char *anna_log_filepath = NULL;
static FILE *anna_log_file = NULL;

static int initialized = 0;
static int stopping = 0;
static int poolsz = 0;
static int maxpoolsz = 32;
/* Buffers over this size will be freed */
static size_t maxbuffersz = 8000;

static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_t printthread;

static anna_log_buffer *printhead = NULL;
static anna_log_buffer *printtail = NULL;
static anna_log_buffer *bufferpool = NULL;


bool anna_log_is_stdout_enabled(void) {
	return anna_log_console;
}

bool anna_log_is_logfile_enabled(void) {
	return anna_log_file != NULL;
}

char *anna_log_get_logfile_path(void) {
	return anna_log_filepath;
}


static void anna_log_freebuffers(anna_log_buffer **list) {
	anna_log_buffer *b, *head = *list;

	while (head) {
		b = head;
		head = b->next;
		free(b);
	}
	*list = NULL;
}

static anna_log_buffer *anna_log_getbuf(void) {
	anna_log_buffer *b;

	pthread_mutex_lock(&lock);
	b = bufferpool;
	if (b) {
		bufferpool = b->next;
		b->next = NULL;
	} else {
		poolsz++;
	}
	pthread_mutex_unlock(&lock);
	if (b == NULL) {
		b = malloc(INITIAL_BUFSZ + sizeof(*b));
		b->allocated = INITIAL_BUFSZ;
		b->next = NULL;
	}
	return b;
}

static void *anna_log_thread(void *ctx) {
	anna_log_buffer *head, *b, *tofree = NULL;

	while (!stopping) {
		pthread_mutex_lock(&lock);
		if (!printhead) {
			pthread_cond_wait(&cond, &lock);
		}
		head = printhead;
		printhead = printtail = NULL;
		pthread_mutex_unlock(&lock);

		if (head) {
			for (b = head; b; b = b->next) {
				if(anna_log_console)
					fputs(b->str, stdout);
				if(anna_log_file)
					fputs(b->str, anna_log_file);
			}
			pthread_mutex_lock(&lock);
			while (head) {
				b = head;
				head = b->next;
				if (poolsz >= maxpoolsz || b->allocated > maxbuffersz) {
					b->next = tofree;
					tofree = b;
					poolsz--;
				} else {
					b->next = bufferpool;
					bufferpool = b;
				}
			}
			pthread_mutex_unlock(&lock);
			if(anna_log_console)
				fflush(stdout);
			if(anna_log_file)
				fflush(anna_log_file);
			anna_log_freebuffers(&tofree);
		}
	}
	/* print any remaining messages, stdout flushed on exit */
	for (b = printhead; b; b = b->next) {
		if(anna_log_console)
			fputs(b->str, stdout);
		if(anna_log_file)
			fputs(b->str, anna_log_file);
	}
	if(anna_log_console)
		fflush(stdout);
	if(anna_log_file)
		fflush(anna_log_file);
	anna_log_freebuffers(&printhead);
	anna_log_freebuffers(&bufferpool);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);

	if(anna_log_file)
		fclose(anna_log_file);
	anna_log_file = NULL;
	free(anna_log_filepath);
	anna_log_filepath = NULL;

	return NULL;
}

void anna_vprintf(const char *format, ...) {
	int len;
	va_list ap, ap2;
	anna_log_buffer *b = anna_log_getbuf();

	va_start(ap, format);
	va_copy(ap2, ap);
	/* first try */
	len = vsnprintf(b->str, b->allocated, format, ap);
	va_end(ap);
	if (len >= (int) b->allocated) {
		/* buffer wasn't big enough */
		b = realloc(b, len + 1 + sizeof(*b));
		b->allocated = len + 1;
		vsnprintf(b->str, b->allocated, format, ap2);
	}
	va_end(ap2);

	pthread_mutex_lock(&lock);
	if (!printhead) {
		printhead = printtail = b;
	} else {
		printtail->next = b;
		printtail = b;
	}
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&lock);
}

int anna_log_init(bool daemon, bool console, const char *logfile) {
	if (initialized) {
		return 0;
	}
	initialized=1;
	//pthread_mutex_init(&lock);
	//pthread_cond_init(&cond);
	if(console) {
		/* Set stdout to block buffering, see BUFSIZ in stdio.h */
		setvbuf(stdout, NULL, _IOFBF, 0);
	}
	anna_log_console = console;
	if(logfile != NULL) {
		/* Open a log file for writing (and append) */
		anna_log_file = fopen(logfile, "awt");
		if(anna_log_file == NULL) {
			printf("Error opening log file %s: %s\n", logfile, strerror(errno));
			return -1;
		}
		anna_log_filepath = strdup(logfile);
	}
	if(!anna_log_console && logfile == NULL) {
		printf("WARNING: logging completely disabled!\n");
		printf("         (no stdout and no logfile, this may not be what you want...)\n");
	}
	if(daemon) {
		/* Replace the standard file descriptors */
		if (freopen("/dev/null", "r", stdin) == NULL) {
			printf("Error replacing stdin with /dev/null\n");
			return -1;
		}
		if (freopen("/dev/null", "w", stdout) == NULL) {
			printf("Error replacing stdout with /dev/null\n");
			return -1;
		}
		if (freopen("/dev/null", "w", stderr) == NULL) {
			printf("Error replacing stderr with /dev/null\n");
			return -1;
		}
	}
	//printthread = g_thread_new(THREAD_NAME, &anna_log_thread, NULL);
	int err = pthread_create(&printthread,NULL,anna_log_thread,NULL);
	if(err != 0){
		printf("Error create thread\n");
	}
	return 0;
}

void anna_log_destroy(void) {
	stopping=1;
	pthread_mutex_lock(&lock);
	/* Signal print thread to print any remaining message */
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&lock);
	pthread_join(printthread,NULL);
}

