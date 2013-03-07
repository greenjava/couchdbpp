/**
 * Copyright 2009 Tragicphantom Productions
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
#include <iostream>
#include <iterator>

#include "couchdb/CouchDB.hpp"

#define TEST_DB_NAME "db_test1"
#define TEST_ID      "id_test1"

using namespace std;

// TODO: debug why copy is not working correctly below
template<typename T>
void output(const vector<T> &v, ostream &out)
{
   typename vector<T>::const_iterator i = v.begin();
   const typename vector<T>::const_iterator &iEnd = v.end();
   for(; i != iEnd; ++i)
      out << *i << " ";
}

static void printDatabases(CouchDB::Connection &conn)
{
   vector<string> dbs = conn.listDatabases();
   copy(dbs.begin(), dbs.end(), ostream_iterator<string>(cout, " "));
   cout << endl;
}

static void printDocuments(CouchDB::Database db)
{
   vector<CouchDB::Document> docs = db.listDocuments();
   cout << db.getName() << " has " << docs.size() << " documents: ";
   //copy(docs.begin(), docs.end(), ostream_iterator<CouchDB::Document>(cout, " "));
   output(docs, cout);
   cout << endl;
}

static void printAttachments(CouchDB::Document &doc)
{
   vector<CouchDB::Attachment> attachments = doc.getAllAttachments();
   cout << "Attachments for document " << doc << ": ";
   //copy(attachments.begin(), attachments.end(), ostream_iterator<CouchDB::Attachment>(cout, " "));
   output(attachments, cout);
   cout << endl;
}

static CouchDB::Variant createRecord(int key, const string &value,
                                   const string &value2, double dValue)
{
   CouchDB::Object obj;
   obj["key"]    = CouchDB::createVariant(key);
   obj["value"]  = CouchDB::createVariant(value);
   obj["value2"] = CouchDB::createVariant(value2);
   obj["dValue"] = CouchDB::createVariant(dValue);
   return CouchDB::createVariant(obj);
}

int main()
{
#ifdef WIN32
   SetEnvironmentVariable("http_proxy", "");
#else
   setenv("http_proxy", "", 1);
#endif

   try
   {
	  const std::string url = "http://stage-rd-6:5984";
      CouchDB::Connection conn(url);
      cout << "CouchDB version: " << conn.getCouchDBVersion() << endl;

      cout << "Initial database set: ";
      printDatabases(conn);

      cout << "Creating test database" << endl;

      conn.createDatabase(TEST_DB_NAME);

      cout << "New database set: ";
      printDatabases(conn);

      CouchDB::Database db = conn.getDatabase(TEST_DB_NAME);

      CouchDB::Array records;
      records.push_back(createRecord(12345, "T1", "DIE", 5.0));
      records.push_back(createRecord(45678, "T2", "C0" , 6.0));
      records.push_back(createRecord(89012, "T3", "C1" , 7.0));

      CouchDB::Object obj;
      obj["x1"]      = CouchDB::createVariant("A12345");
      obj["x2"]      = CouchDB::createVariant("ABCDEF");
      obj["records"] = CouchDB::createVariant(records);

      CouchDB::Variant data = CouchDB::createVariant(obj);
      cout << "Creating new document" << endl;
      CouchDB::Document doc = db.createDocument(data);
      cout << "Created doc: " << doc << endl;

      printDocuments(db);

      CouchDB::Document testDoc = db.getDocument(doc.getID());

      cout << "Document in database: "
           << testDoc << ": "
           << testDoc.getData() << endl;

      cout << "Comparing docs: " << doc << " and " << testDoc << endl;

      if(doc == testDoc)
         cout << "Verified documents are the same" << endl;
      else
         cout << "Documents differ:" << endl
              << doc.getData() << endl
              << testDoc.getData() << endl;

      cout << "Creating a version of the document with id: " << TEST_ID << endl;
      CouchDB::Document docId = db.createDocument(data, TEST_ID);
      cout << "Created new document: " << docId << endl;

      docId.addAttachment("h1", "text/plain", "hello world");
      docId.addAttachment("h2", "text/plain", "cool world");

      CouchDB::Attachment a = docId.getAttachment("h1");
      cout << "Got attachment: " << a << ": " << a.getData() << endl;

      printAttachments(docId);

      cout << "Removing attachment: " << a << endl;
      docId.removeAttachment(a.getID());

      printAttachments(docId);

      cout << "Trying to access removed attachment: " << a << endl;

      try
	  {
         string badData = a.getData();
         cout << "ERROR: Got valid data for removed attachment: " << badData << endl;
      }
      catch(CouchDB::Exception &e)
	  {
         cerr << "Unable to retrieve data for removed attachment (GOOD): " << e.what() << endl;
      }

      docId.remove();

      cout << "Copying document: " << doc << endl;
      CouchDB::Document copyDoc = doc.copy(TEST_ID);
      cout << "Got document copy: " << copyDoc << endl;
      copyDoc.remove();

      doc.remove();

      cout << "Docs removed" << endl;

      printDocuments(db);

      cout << "Trying to retrieve data for removed document: " << testDoc << endl;

      try
	  {
         data = testDoc.getData();
         cout << "ERROR: Got valid data for removed document: " << data << endl;
      }
      catch(CouchDB::Exception &e)
	  {
         cerr << "Unable to retrieve data for removed document (GOOD): " << e.what() << endl;
      }

      cout << "Removing test database" << endl;

      conn.deleteDatabase(TEST_DB_NAME);

      cout << "Final database set: ";
      printDatabases(conn);

      cout << "All tests passed" << endl;
   }
   catch(exception &e)
   {
      cerr << "Exception: " << e.what() << endl;
   }
   catch(...)
   {
      cerr << "Caught unknown exception in main..." << endl;
   }
}
