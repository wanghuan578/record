
#include "event/event.h"
#include "event/evhttp.h"
#include "event/event2/http.h"
#include "json/json.h"
#include "record_control.h"
#include "process_manager.h"
#include <string.h>
#include <assert.h>

using namespace std;
using namespace p_base;



namespace p_surf {

	CRecordCtl::CRecordCtl() {

	}

	CRecordCtl::~CRecordCtl() {

	    delete app_data.port;
	    delete app_data.root_path;
	}

	int CRecordCtl::init(G_CONF_S *g_conf) {
		
		app_data.port = pconf::get_value_from_node(g_conf->mod_tree, "port");

		if(NULL == app_data.port) {

			printf("port unspecified\n");

			exit(0);
		}

		app_data.root_path = pconf::get_value_from_node(g_conf->mod_tree, "root");

                if(NULL == app_data.root_path) {

			printf("root path unspecified\n");

                        exit(0);
                }
		
		app_data.host = pconf::get_value_from_node(g_conf->mod_tree, "host");

                if(NULL == app_data.host) {

                        printf("host unspecified\n");

                        exit(0);
                }

		return 0;
	}
	
	int CRecordCtl::exec() {
		
		struct evhttp *httpd;
		
		event_init();
		
		httpd = evhttp_start("0.0.0.0", atoi(app_data.port));
		
		if(NULL == httpd) {

			printf("port bind already\n");

			exit(0);
		}

		evhttp_set_cb(httpd, "/api/r3/start", start_record, (void*)(&app_data));
		evhttp_set_cb(httpd, "/api/r3/stop", stop_record, (void*)(&app_data));
		evhttp_set_cb(httpd, "/api/r3/create", create_record, (void*)(&app_data));
		evhttp_set_cb(httpd, "/api/r3/delete", remove_record, (void*)(&app_data));

		//evhttp_set_gencb(httpd, generic_handler, NULL);
		
		printf("listen prot %s ...\n", app_data.port);
	
		event_dispatch();
		
		evhttp_free(httpd);
		
		return 0;
	}

	int CRecordCtl::stop() {

  		printf("libevent cycle terminal\n");
		
		event_loopbreak();

 		return 0;
	}

	void CRecordCtl::create_record(struct evhttp_request *req, void *arg)
	{
		int ret = 0;

                string output;
                struct evbuffer *buffer;

                Json::Reader reader;
                Json::Value root;

		_m3u8_param param;
                char tmp[256] = {0};
                char *body = NULL;
		string path;
		time_t t;
		size_t pos = string::npos;

		_app_data *data = (_app_data*) arg;

		assert(NULL != data);

                buffer = evbuffer_new();

                body = (char *) EVBUFFER_DATA(req->input_buffer);

                if(NULL == body) {

                    goto HTTP_BODY_NULL;
                }

                snprintf(tmp, sizeof(tmp), "post_data=%s\n", body);
                printf("%s\n", tmp);

                if (!reader.parse(body, root, false))
                {
                    goto INPUT_NO_MATCH;
                }
		
		path = root["path"].asString();

		pos = path.rfind("/");

    	    	if (pos != std::string::npos) {
        	    path = path.substr(0, pos);
    		}

		printf("path = %s\n", path.c_str());

                param.sid = root["streamcode"].asString();

		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%s%s/", data->root_path, path.c_str());
		param.path = tmp;

                ret = ProcessManager::Instance()->create_m3u8(param);
		
		if(REC_ERROR_CREATE_M3U8_FILE == ret) {

		    goto CREATE_FILE_FAILED;
		}

                output.append("{\"ret\":");
                output.append("0");
                output.append(",\"");
                output.append("msg\":\"");
                output.append("create task start\",\"path\":\"");
		output.append(param.file_path.c_str());
		output.append("\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, HTTP_OK, "OK", buffer);
                evbuffer_free(buffer);
                return;

	CREATE_FILE_FAILED:
		memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%d", ret);
                output.append("{\"ret\":");
                output.append(tmp);
                output.append(",\"");
                output.append("msg\":\"");
                output.append("create task failed\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "paramter not match", buffer);
                evbuffer_free(buffer);
                return;

        INPUT_NO_MATCH:
                memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%d", ret);
                output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("input not match\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "paramter not match", buffer);
                evbuffer_free(buffer);
                return;

         HTTP_BODY_NULL:
                memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%d", 400);
                output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("http post body null\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "http body null", buffer);
                evbuffer_free(buffer);
                return;
	}

	void CRecordCtl::remove_record(struct evhttp_request *req, void *arg)
        {

        }

	void CRecordCtl::start_record(struct evhttp_request *req, void *arg)
	{
		int ret = 0;
		string output;
		string path;
		struct evbuffer *buffer = NULL;
		_process_info cmd_param;
		Json::Reader reader;
                Json::Value root;
		char tmp[256] = {0};
		char *body = NULL;

		_app_data *data = (_app_data*) arg;

		assert(NULL != data);

		buffer = evbuffer_new();

		memset(tmp, 0x00, sizeof(tmp));
		body = (char *) EVBUFFER_DATA(req->input_buffer);

		
                if(NULL == body) {

                    goto HTTP_BODY_NULL;
                }

        	snprintf(tmp, sizeof(tmp), "post_data=%s\n", body);
		printf("%s\n", tmp);

		if (!reader.parse(body, root, false))
		{
		    goto INPUT_NO_MATCH;
		}

		cmd_param.src_url = root["src"].asString();
		
		path = root["path"].asString();
		memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%s%s", data->root_path, path.c_str());
                cmd_param.m3u8_path = tmp;

		cmd_param.sid = root["streamcode"].asString();
		cmd_param.type = root["type"].asString();
 		cmd_param.status = "active";
		cmd_param.host = data->host;
		
		printf("@@@@@@@@@@@@@@cmd_param m3u8 path %s\n", cmd_param.m3u8_path.c_str());
	
		ret = ProcessManager::Instance()->put_data(cmd_param);
		
		if(REC_ERROR_STREAM_EXIST == ret) {
		    
		    goto STREAM_EXIST;
		}

		output.append("{\"error_code\":");
    		output.append("0");
    		output.append(",\"");
    		output.append("error_text\":\"");
    		output.append("SUCCESS\"}");
		
		evbuffer_add_printf(buffer, "%s", output.c_str());
    		evhttp_send_reply(req, HTTP_OK, "OK", buffer);
    		evbuffer_free(buffer);
    		return;

	STREAM_EXIST:
		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", ret);
		output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("stream exist\"}");
                evbuffer_add_printf(buffer, "%s", output.c_str());

                evhttp_send_reply(req, 403, "stream exist", buffer);
                evbuffer_free(buffer);
                return;

	INPUT_NO_MATCH:
		memset(tmp, 0x00, sizeof(tmp));
		snprintf(tmp, sizeof(tmp), "%d", ret);
		output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("input not match\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "paramter not match", buffer);
                evbuffer_free(buffer);
		return;

	 HTTP_BODY_NULL:
                memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%d", 400);
                output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("http post body null\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "http body null", buffer);
                evbuffer_free(buffer);
                return;
	}

	void CRecordCtl::stop_record(struct evhttp_request *req, void *arg)
	{
		int ret = 0;
		string output;
                struct evbuffer *buffer;
                _process_info cmd_param;
		Json::Reader reader;
                Json::Value root;
		char tmp[256];
		char *body;

		_app_data *data = (_app_data*) arg;

                buffer = evbuffer_new();

                memset(tmp, 0x00, sizeof(tmp));
		body = (char *) EVBUFFER_DATA(req->input_buffer);

		if(NULL == body) {

		    goto HTTP_BODY_NULL; 
		}

                snprintf(tmp, sizeof(tmp), "post_data=%s\n", body);
                printf("%s\n", tmp);
		printf("CRecordCtl::stop_record\n");

                if (!reader.parse(body, root, false))
                {
                        goto INPUT_NO_MATCH;
                }

                cmd_param.src_url = root["src"].asString();
                cmd_param.sid = root["streamcode"].asString();

                ret = ProcessManager::Instance()->kill_process(cmd_param);

		if(REC_ERROR_STREAM_NOT_EXIST == ret) {

                    goto STREAM_NOT_EXIST;
                }

                output.append("{\"error_code\":");
                output.append("0");
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("SUCCESS\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, HTTP_OK, "OK", buffer);
                evbuffer_free(buffer);
                return;

	INPUT_NO_MATCH:
		memset(tmp, 0x00, sizeof(tmp));
 		snprintf(tmp, sizeof(tmp), "%d", ret);
		output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("input not match\"}");

		evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "paramter not match", buffer);
                evbuffer_free(buffer);
		return;

	STREAM_NOT_EXIST:
                memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%d", ret);
                output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("stream not exist\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 404, "stream not exist", buffer);
                evbuffer_free(buffer);
                return;
	
        HTTP_BODY_NULL:
                memset(tmp, 0x00, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%d", 400);
                output.append("{\"error_code\":");
                output.append(tmp);
                output.append(",\"");
                output.append("error_text\":\"");
                output.append("http post body null\"}");

                evbuffer_add_printf(buffer, "%s", output.c_str());
                evhttp_send_reply(req, 400, "http body null", buffer);
                evbuffer_free(buffer);
                return;
	}
}
