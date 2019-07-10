
#include "mongo_adaptor.h"
#include <mongoc.h>
#include <bson.h>
#include "common.h"

using namespace p_surf;

MongoAdaptor::MongoAdaptor()
{
}

MongoAdaptor::~MongoAdaptor()
{
}

int MongoAdaptor::init()
{
    mongoc_init();
    printf("MongoAdaptor::init\n"); 
    return 0;
}

void *MongoAdaptor::connect(string addr)
{
    return static_cast<void *>(mongoc_client_new(addr.c_str()));
}

void MongoAdaptor::connect_close(void *c)
{
    mongoc_client_t *client = static_cast<mongoc_client_t *>(c);

    if(NULL != client) {
  	
	mongoc_client_destroy(client);
    }
}

void *MongoAdaptor::collection(void *connect, string &db, string &collection)
{
    mongoc_client_t *client = static_cast<mongoc_client_t *>(connect);

    if(NULL == client) {

	return NULL;
    }

    return (void*)(mongoc_client_get_collection(client, db.c_str(), collection.c_str()));
}

void MongoAdaptor::collection_release(void *collection)
{
    mongoc_collection_t *collec = static_cast<mongoc_collection_t *>(collection);
    
    if(NULL != collec) {

    	mongoc_collection_destroy(collec);
    }
}

bool MongoAdaptor::insert(void *collection, bson_t *query)
{
    bson_error_t error;
    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if (NULL == collect) {

	return false;
    }

    return mongoc_collection_insert(collect, MONGOC_INSERT_NONE, query, NULL, &error);
}

bool MongoAdaptor::update(void *collection, bson_t *query, bson_t *update)
{
    bson_error_t error;

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);
    
    if(NULL == collect) {

        return false;
    }

    return mongoc_collection_update(collect, MONGOC_UPDATE_NONE, query, update, NULL, &error);
}

int MongoAdaptor::find(void *collection, bson_t *query, vector<string> &list)
{
    const bson_t *doc;
    char *str;
    vector<string> set;

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if(NULL == collect) {

        return 1;
    }

    mongoc_cursor_t *cursor = mongoc_collection_find (collect, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
    
    while (mongoc_cursor_next(cursor, &doc)) {

	str = bson_as_json (doc, NULL);
	
	list.push_back(str);

	bson_free (str);
    }

    return 0;
}

int MongoAdaptor::findOne(void *collection, bson_t *query, string &item)
{
    const bson_t *doc;
    char *str;

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if(NULL == collect) {

        return REC_ERROR_GET_COLLECTION_FAILED;
    }

    mongoc_cursor_t *cursor = mongoc_collection_find (collect, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);

    while (mongoc_cursor_next(cursor, &doc)) {

        str = bson_as_json (doc, NULL);

        item = str;

	printf("wanghuan --- MongoAdaptor::findOne item = %s\n", item.c_str());

        bson_free (str);

	return REC_ERROR_OK;
    }

    printf("wanghuan --- MongoAdaptor::findOne none\n");
    return REC_ERROR_PORCESS_INFO_NONE;
}

bool MongoAdaptor::remove(void *collection, bson_t *query)
{
    bson_error_t error;
    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if(NULL == collect) {

	return false;
    }

    return mongoc_collection_remove(collect, MONGOC_REMOVE_SINGLE_REMOVE, query, NULL, &error);
}
