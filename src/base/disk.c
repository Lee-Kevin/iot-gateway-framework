/*******************************************************************************
 * Author			: liang | work at ym.
 * Email			: 944922198@qq.com
 * Last modified	: 2017-01-23 15:18
 * Filename			: disk.c
 * Description		: 
 * *****************************************************************************/
#include <sqlite3.h>
#include <stdio.h>
#include <uuid/uuid.h>
#include "disk.h"
#include "common.h"
static sqlite3 *pdb = NULL;
char uuid[64]={0};
int disk_init(void *param)
{
    int ret;
    char s_uuid[128]={0};
    uuid_t uuid;
    char *sql=NULL;
    int cnt;
    
	char dbname[256]={0};
    memcpy(dbname,(char*)param,256);
    ret = sqlite3_open_v2(dbname,&pdb, SQLITE_OPEN_READWRITE| SQLITE_OPEN_CREATE, NULL);
    if(SQLITE_OK != ret){
        return -1;
    }

    sqlite3_stmt* stmt = NULL;
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='SYSTEM'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if (ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table SYSTEM(key text not null, value text not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            uuid_generate(uuid);
            uuid_unparse_upper(uuid, s_uuid);
            sqlite3_prepare(pdb,"insert into SYSTEM(key,value) values(?,?)",-1,&stmt,NULL);
            sqlite3_bind_text(stmt, 1,"uuid", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, s_uuid, -1, SQLITE_STATIC);
            sqlite3_step(stmt);

            sqlite3_prepare(pdb,"insert into SYSTEM(key,value) values(?,?)",-1,&stmt,NULL);
            sqlite3_bind_text(stmt, 1,"name", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2,"未命名", -1, SQLITE_STATIC);
            sqlite3_step(stmt);

            sqlite3_prepare(pdb,"insert into SYSTEM(key,value) values(?,?)",-1,&stmt,NULL);
            sqlite3_bind_text(stmt, 1,"lastmodify", -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2,time(0));
            sqlite3_step(stmt);
            
            sqlite3_finalize(stmt);
        }else{
        //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

    /*create table USER*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='USER'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table USER(name text not null,passwd text not null,auth not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
                //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

    /*create table SESSION*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='SESSION'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table SESSION(session text not null,name text not null,lifetime int not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
                //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

    /*create table AREA*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='AREA'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            /*dev_type: 2byte class field and 2byte device field,0x00010002 ,0x0001 is class id,0x0002 is device type*/
            sqlite3_prepare(pdb,"create table AREA(floorid int not null,floorname text not null,roomid int not null,roomname text not null,areaid int not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
                //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

    /*create table MODBUS*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='MODBUS'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);         
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table MODBUS(areaid int not null,devid int not null,dev_name text not null,devtype int not null,maddr int not null,saddr int not null,dpid int not null,dpname text not null,dptype int not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
                //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

    /*create table DALITEK_DEV_STATUS*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='DEV_STATUS'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table DEV_STATUS(dev_id int not null,dp_id int not null, value integer not null,grp_flag int not null,timestamp int not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
        //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }


/*look  here */
	/*create table DALITEK_SCENE*/
	sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='SCENES'";
	sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
	ret = sqlite3_step(stmt);
	cnt = 0;
	if(ret == SQLITE_ROW){
		cnt = sqlite3_column_int(stmt,0);
		sqlite3_finalize(stmt);
		if(cnt == 0){
                sqlite3_prepare(pdb,"create table SCENES(id int not null,scenes text not null)", -1, &stmt, NULL);
                sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}else{
                //to do update...
		}
	}else{
		sqlite3_finalize(stmt);
	}
	
    /*create table DALITEK_TIMED*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='TIMED'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table TIMED(id int not null, timedtask text not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
    //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

        /*create table DALITEK_GROUP*/
    sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='GROUP'";
    sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
    ret = sqlite3_step(stmt);
    cnt = 0;
    if(ret == SQLITE_ROW){
        cnt = sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        if(cnt == 0){
            sqlite3_prepare(pdb,"create table GROUP(grp_id int not null, dev_id int not null,dp_id int not null)", -1, &stmt, NULL);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }else{
            //to do update...
        }
    }else{
        sqlite3_finalize(stmt);
    }

    /*create table DALITEK_LINKAGE*/
	sql = "select COUNT(*) as cnt from sqlite_master where type='table' and name='LINKAGE'";
	sqlite3_prepare_v2(pdb, sql, strlen(sql), &stmt, NULL);
	ret = sqlite3_step(stmt);
	cnt = 0;
	if(ret == SQLITE_ROW){
		cnt = sqlite3_column_int(stmt,0);
		sqlite3_finalize(stmt);
		if(cnt == 0){
			sqlite3_prepare(pdb,"create table LINKAGE(id int not null,timestamp int not null,linkages text not null)", -1, &stmt, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}else{
			//to do update...
		}
	}else{
		sqlite3_finalize(stmt);
	} 
    //DALITEK_DEBUG(DBG,"database init success!\n");
    return 0;
}
int disk_exit(void)
{
	if(pdb != NULL){
			sqlite3_close(pdb);
			pdb = NULL;
	}
	return 0;
}
char *disk_get_uuid(void)
{
	//static char uuid[128]={0};
    sqlite3_stmt* stmt = NULL;
	if(pdb!=NULL){
    	sqlite3_prepare(pdb,"select value from SYSTEM  where key='uuid'",-1,&stmt,NULL);
		sqlite3_step(stmt);
		strncpy(uuid,(const char*)sqlite3_column_text( stmt,0),64);
        sqlite3_finalize(stmt);
	}
	return uuid;
}

int disk_user_register(const char *username,const char *passwd,const char *tel,const char *email,const char *idcard)
{
    sqlite3_stmt* stmt = NULL;
    int max;
	if(pdb!=NULL){
    	sqlite3_prepare(pdb,"select value from SYSTEM  where key='uuid'",-1,&stmt,NULL);
		sqlite3_step(stmt);
		strncpy(uuid,(const char*)sqlite3_column_text( stmt,0),64);
        sqlite3_finalize(stmt);
	}
}
/*################################ user start#####################################*/

int disk_user_get_items(struct user_item *p_user)
{
    int err_code=ERR_SUCCESS;
    int ret=-1;
    sqlite3_stmt* stmt = NULL;
    if(pdb == NULL){
        return ERR_NO_DB;
    }
    
    sqlite3_prepare(pdb,"select name,passwd from USER",-1,&stmt,NULL);
    ret = sqlite3_step(stmt);
    while(ret == SQLITE_ROW){
        char name[16]={0};
        char passwd[16]={0};
        strncpy(name,(const char *)sqlite3_column_text(stmt,0),sizeof(name)-1);
        strncpy(passwd,(const char *)sqlite3_column_text(stmt,1),sizeof(passwd)-1);
        user_item_t *s1,*s2;
        HASH_FIND_STR(p_user,name,s1);
        if(!s1){
            s2 = (user_item_t *)malloc(sizeof(user_item_t));
            if(s2){
                strncpy(s2->name,name,sizeof(s2->name)-1);
                strncpy(s2->passwd,passwd,sizeof(s2->passwd)-1);
                HASH_ADD_STR(p_user,name,s2);
            }
        }
        ret = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    return ERR_SUCCESS;
}

struct user_item *disk_user_get_item(const char *name,struct user_item *p_user)
{
    if(p_user == NULL)
        return NULL;
    int ret=-1;
    sqlite3_stmt* stmt = NULL;
    if(pdb == NULL){
        return NULL;
    }
    
    sqlite3_prepare(pdb,"select name,passwd from USER where name=?",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    ret = sqlite3_step(stmt);
    if(ret == SQLITE_ROW){
        char name[16]={0};
        char passwd[16]={0};
        strncpy(p_user->name,(const char *)sqlite3_column_text(stmt,0),sizeof(p_user->name)-1);
        strncpy(p_user->passwd,(const char *)sqlite3_column_text(stmt,1),sizeof(p_user->passwd)-1);
        sqlite3_finalize(stmt);
        return p_user;
    }
    sqlite3_finalize(stmt);
    return NULL;
}


int disk_user_add(const char *name, const char *passwd)
{
    if(name == NULL)
        return ERR_PARAMTER;
    int err_code=ERR_SUCCESS;
    int ret=-1;
    sqlite3_stmt* stmt = NULL;
    if(pdb == NULL){
        return ERR_NO_DB;
    }
    sqlite3_prepare(pdb,"select * from USER  where username=? and passwd=?",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,passwd, -1, SQLITE_STATIC);
    ret = sqlite3_step(stmt);
    if(ret == SQLITE_ROW){
        return ERR_USER_EXITED;
    }
    sqlite3_finalize(stmt);
    
    sqlite3_prepare(pdb,"insert into USER(name,passwd) values(?,?)",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,passwd, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return ERR_SUCCESS;

}
//删除用户
int disk_user_delete(const char *name,const char *passwd)
{
    if(name == NULL)
        return ERR_PARAMTER;
    int err_code=ERR_SUCCESS;
    int ret=-1;
    sqlite3_stmt* stmt = NULL;
    if(pdb == NULL){
        return ERR_NO_DB;
    }
    sqlite3_prepare(pdb,"select * from USER  where name=? and passwd=?",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,passwd, -1, SQLITE_STATIC);
    ret = sqlite3_step(stmt);
    if(ret != SQLITE_ROW){
        return ERR_NO_USER;
    }
    sqlite3_finalize(stmt);
    
    sqlite3_prepare(pdb,"delete from USER where name=? and passwd=?",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,passwd, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return ERR_SUCCESS;
}


//修改用户密码
int disk_user_modify_passwd(const char *name,const char *old_passwd,const char *new_passwd)
{
    if(name == NULL)
        return ERR_PARAMTER;
    int err_code=ERR_SUCCESS;
    int ret=-1;
    sqlite3_stmt* stmt = NULL;
    if(pdb == NULL){
        return ERR_NO_DB;
    }
    sqlite3_prepare(pdb,"select * from USER  where name=? and passwd=?",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,old_passwd, -1, SQLITE_STATIC);
    ret = sqlite3_step(stmt);
    if(ret != SQLITE_ROW){
        return ERR_NO_USER;
    }
    sqlite3_finalize(stmt);
    
    sqlite3_prepare(pdb,"update USER set passwd=? where name=? ",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,new_passwd, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return ERR_SUCCESS;
}
//修改用户昵称
int disk_user_modify_name(const char *name,const char *passwd,const char *new_name)
{
    if(name == NULL)
        return ERR_PARAMTER;
    int err_code=ERR_SUCCESS;
    int ret=-1;
    sqlite3_stmt* stmt = NULL;
    if(pdb == NULL){
        return ERR_NO_DB;
    }
    sqlite3_prepare(pdb,"select * from USER  where name=? and passwd=?",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,passwd, -1, SQLITE_STATIC);
    ret = sqlite3_step(stmt);
    if(ret != SQLITE_ROW){
        return ERR_NO_USER;
    }
    sqlite3_finalize(stmt);
    
    sqlite3_prepare(pdb,"update USER set name=? where passwd=? ",-1,&stmt,NULL);
    sqlite3_bind_text(stmt, 1,new_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,passwd, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return ERR_SUCCESS;
}


/*################################ user end #####################################*/

