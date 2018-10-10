
struct user_item *g_users = NULL;
struct session_item *g_sessions = NULL;
/**/
int user_init()
{
	if(disk_user_get_items(g_users) < 0)
		return -1;
	if(disk_session_get_items(g_session) < 0)
		return -1;
	return 0;
	
}
static m_user_destroy()
{
	struct user_item *s,*tmp;
	HASH_ITER(hh,g_users,s,tmp){
		HASH_DEL(g_users,s);
		free(s);
	}
}
static m_session_destroy()
{
	struct user_session *s,*tmp;
	HASH_ITER(hh,g_sessions,s,tmp){
		HASH_DEL(g_sessions,s);
		free(s);
	}
}
void user_uninit()
{
	m_user_destroy();
	m_session_destroy();
}



bool user_add(const char *name,const char *passwd)
{
	if(name == NULL || passwd == NULL)
		return false;
	struct user_item *s;
	s = (struct user_item *)malloc(sizeof(struct user_item));
	if(s == NULL){
		/*out of memory */
		return false;
	}
	memset(s,0,sizeof(struct user_item));
	snprintf(s->name,name,sizeof(s->name)-1);
	snprintf(s->passwd,passwd,sizeof(s->passwd)-1);
	HASH_ADD_STR( g_users, name, s );
	
	/*disk operation*/
	disk_user_add(name,passwd);
	
	return 0;
}


bool user_del(const char *name,const char *passwd)
{
	if(name == NULL || passwd == NULL)
		return false;
	struct user_item *s = NULL;
	HASH_FIND_STR(g_users, name, s);
	if(s){
		if(!strcmp(s->passwd,passwd)){
			HASH_DEL(g_users,s);
			free(s);
			/*disk operation*/
			disk_user_del();
		}
	}
	return true;
}
struct user_item *user_get_info(const char *name)
{
	if(name == NULL)
		return NULL;
	struct user_item *s = NULL;
	HASH_FIND_STR(g_users, name, s);
	if(s){
		return s;
	}
	return NULL;
}

int user_modify_passwd(const char *name,const char *old_passwd,const char new_passwd)
{
	if(name == NULL || passwd == NULL)
		return -1;
	struct user_item *s = NULL;
	HASH_FIND_STR(g_users, name, s);
	if(s){
		if(!strcmp(s->passwd,old_passwd)){
			if(!strcmp(old_passwd,new_passwd)){
				return 1;//新旧密码一样，提示用户
			}
			memset(s->passwd,0,sizeof(s->passwd));
			snprintf(s->passwd,new_passwd,sizeof(s->passwd)-1);
			/*disk operation*/
			disk_user_modify_passwd();
			return 0;//修改成功
		}else {
			return 2;//旧密码验证失败，提示用户重新填写密码或找回密码
		}
	}
	return 2;//用户名错误
}



bool session_add(const char *session,int lifetime)
{
	if(session == NULL)
		return false;
	struct session_item *s;
	s = (struct session_item *)malloc(sizeof(struct session_item));
	if(s == NULL){
		/*out of memory */
		return false;
	}
	memset(s,0,sizeof(struct session_item));
	snprintf(s->session,session,sizeof(s->session)-1);
	s->lifetime = lifetime;
	HASH_ADD_STR(g_sessions,session,s);
	
	/*disk operation*/
	disk_session_add(session,lifetime);
	
	return true;
}

int session_del(const char *session)
{
	if(session == NULL)
		return -1;
	struct user_item *s = NULL;
	HASH_FIND_STR(g_sessions, session, s);
	if(s){
		HASH_DEL(g_sessions,s);
		free(s);
		
		/*disk operation*/
		disk_session_del(session);
		return 0;/*删除成功*/
	}
	return 1;/*不存在*/
}

struct session_item *session_get_info(const char *session)
{
	if(session == NULL)
		return NULL;
	struct session_item *s = NULL;
	HASH_FIND_STR(g_sessions, session, s);
	if(s){
		return s;
	}
	return NULL;
	
}

bool session_is_legal(const char *session)
{
	if(session == NULL)
		return false;
	struct session_item *s = NULL;
	HASH_FIND_STR(g_sessions, session, s);
	if(s){
		if(s->lifetime > 0)
			return true;
	}
	return false;
}

bool session_modify_second();


