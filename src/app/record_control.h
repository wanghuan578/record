
#ifndef __SP_RECORD_CTL_H__
#define __SP_RECORD_CTL_H__


#include "common.h"
//#include "lock/pure_lock.h"
#include "global_conf.h"

namespace p_surf
{
	class CRecordCtl
	{
	public:
		CRecordCtl();
		~CRecordCtl();
		
		int init(G_CONF_S *g_conf);
		int exec();
		int stop();

		static void start_record(struct evhttp_request *req, void *arg);
		static void stop_record(struct evhttp_request *req, void *arg);
		static void create_record(struct evhttp_request *req, void *arg);
		static void remove_record(struct evhttp_request *req, void *arg);
	private:
		_app_data app_data;
		//char *host;
		//char *port;
		//char *root_path;
	};
}


#endif
