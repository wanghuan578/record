#include "usage.h"

char g_usage[] = "Usage: "PROGRAM"[-?hdv] [-s signal] [-t filename] [-c filename] " ENDLINE
				ENDLINE
				"Options:" ENDLINE
				"  -?,-h         : this help" ENDLINE
				"  -d            : debug mode" ENDLINE
				"  -v            : show version and exit" ENDLINE
				"  -t filename   : test configuration and exit,default:" DEFAULT_CONF_PATH ENDLINE
				"  -s signal : send signal to process: "
				   				"stop, reload" ENDLINE
				"  -c filename   : set configuration and start" ENDLINE;


int get_argument(int argc,char *const * argv,struct argument & argu_s)
{
	
	
	for(int i=1;i<argc;i++)
	{
		char * p = argv[i];

		//start with '-'
		if(p[0] != '-')
		{
			printf("invalid argument %d:%s \n",i,p);
			return false;
		}

		switch(p[1])
		{
		case '?':
		case 'h':
			argu_s.show_help = true;
			break;
		case 'd':
			argu_s.debug_mode = true;
			break;
		case 'v':
			argu_s.show_version = true;
			break;
		case 't':
			//test config
			argu_s.test_config = true;
			argu_s.test_conf_s = DEFAULT_CONF_PATH;
			if(i+1<argc)
			{
				if(argv[i+1][0] != '-')
				{
					argu_s.test_conf_s = argv[i+1];
					i++;
				}
			}
			break;
	/*	case 'r':
			//test license
			argu_s.test_license = true;
			argu_s.test_lic_s = DEFAULT_LICENSE_PATH;
			if(i+1<argc)
			{
				if(argv[i+1][0] != '-')
				{
					argu_s.test_lic_s = argv[i+1];
					i++;
				}
			}
			break;*/
		case 'c':
			//set config
			argu_s.set_config = true;
			argu_s.set_conf_s = DEFAULT_CONF_PATH;
			if(i+1<argc)
			{
				if(argv[i+1][0] != '-')
				{
					argu_s.set_conf_s = argv[i+1];
					i++;
				}
			}
			break;
		case 's':
			//send signal
			argu_s.send_signal = true;
			if(i+1<argc)
			{
				if(argv[i+1][0] != '-')
				{
					switch(argv[i+1][0])
					{
					case 's':
						argu_s.send_sig_s = SIGNAL_STOP;
						break;
					case 'r':
						argu_s.send_sig_s = SIGNAL_RELOAD;
						break;
					default:
						printf("invalid argument %d:%s unknown signal type\n",i+2,argv[i+2]);
						return false;
					}
					i++;
				}
				else
				{
					printf("-s need argument: signal type:stop or reload\n");
					return false;
				}
			}
			else
			{
				printf("-s need arguments:module and signal type:stop or reload\n");
				return false;
			}
			
			break;
		default:
			printf("unknown argument %d:%s",i,argv[i]);
			return false;
		}
		
	}
	return true;
}


