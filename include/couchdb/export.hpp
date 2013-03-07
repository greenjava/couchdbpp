#ifndef __COUCH_DB_CONFIG_HPP__
#define __COUCH_DB_CONFIG_HPP__

#ifdef _WIN32
    #ifdef COUCHDB_EXPORTS
        #define COUCHDB_API __declspec(dllexport)
    #else
        #define COUCHDB_API __declspec(dllimport)
    #endif
    #define COUCHDB_CLASS_API
#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)
    #ifdef COUCHDB_EXPORTS
        #define COUCHDB_API __attribute__ ((visibility("default")))
        #define COUCHDB_CLASS_API __attribute__ ((visibility("default")))
    #else
        #define COUCHDB_API __attribute__ ((visibility("hidden")))
        #define COUCHDB_CLASS_API __attribute__ ((visibility("hidden")))
    #endif
#else
    #define COUCHDB_API
    #define COUCHDB_CLASS_API
#endif

#endif //__COUCH_DB_CONFIG_HPP__

