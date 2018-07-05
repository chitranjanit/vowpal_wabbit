﻿#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif

#include <boost/test/unit_test.hpp>
#include <unordered_map>
#include "model_mgmt.h"
#include "object_factory.h"
#include "factory_resolver.h"
#include "constants.h"
#include "api_status.h"
#include "err_constants.h"
#include <regex>
#include "periodic_background_proc.h"
#include "model_downloader.h"
#include "data_callback_fn.h"
<<<<<<< HEAD
#include "http_server/http_server.h"
#include "config_utility.h"
#include "config_collection.h"
=======
>>>>>>> master

namespace r = reinforcement_learning;
namespace m = reinforcement_learning::model_management;
namespace u =  reinforcement_learning::utility;
namespace e = reinforcement_learning::error_code;
<<<<<<< HEAD
namespace cfg = reinforcement_learning::utility::config;

const auto JSON_CFG = R"(
  {
    "ApplicationID": "rnc-123456-a",
    "EventHubInteractionConnectionString": "Endpoint=sb://localhost:8080/;SharedAccessKeyName=RMSAKey;SharedAccessKey=<ASharedAccessKey>=;EntityPath=interaction",
    "EventHubObservationConnectionString": "Endpoint=sb://localhost:8080/;SharedAccessKeyName=RMSAKey;SharedAccessKey=<ASharedAccessKey>=;EntityPath=observation",
    "IsExplorationEnabled": true,
    "ModelBlobUri": "http://localhost:8080",
    "InitialExplorationEpsilon": 1.0
  }
  )";
const auto JSON_CONTEXT = R"({"_multi":[{},{}]})";
=======

const char * A_BLOB_URI   = "https://storagex6ilrr3obwyra.blob.core.windows.net/mwt-models/current?sv=2017-04-17&sr=b&sig=n9udcFiyjhahgnK38elMIEbg9FYWfmmAWte1MP87v1s%3D&st=2018-03-19T17%3A51%3A18Z&se=2028-03-19T17%3A52%3A18Z&sp=r";
const char * CFG_VALUE = "model.vw";
>>>>>>> master

m::model_data get_model_data();

int get_export_frequency(const u::config_collection& cc, int& interval_ms, r::api_status* status) {
  const auto export_freq_s = cc.get("ModelExportFrequency", nullptr);
  if ( export_freq_s == nullptr ) {
<<<<<<< HEAD
    RETURN_ERROR_LS(status, model_export_frequency_not_provided);
=======
    RETURN_ERROR(status, e::model_export_frequency_not_provided, e::model_export_frequency_not_provided_s);
>>>>>>> master
  }
  // hh:mm:ss
  const std::regex re("\\s*([0-9]+):([0-9]+):([0-9]+)\\s*");
  std::cmatch m;
  if(std::regex_match(export_freq_s,m,re)) {
    const auto hrs  = atoi(m[1].str().c_str());
    const auto mins = atoi(m[2].str().c_str());
    const auto secs = atoi(m[3].str().c_str());
    interval_ms = hrs * 60 * 60 * 1000 + mins * 60 * 1000 + secs * 1000;
<<<<<<< HEAD
    if ( interval_ms == 0 ) {
      RETURN_ERROR_LS(status, bad_time_interval);
    }
    return e::success;
  }
  else {
    RETURN_ERROR_LS(status, bad_time_interval);
  }
}

void dummy_error_fn(const r::api_status& err, void* ctxt) {
  *( (int*)ctxt ) = 10;
}

void dummy_data_fn(const m::model_data& data, int* ctxt) {
  *ctxt = 20;
}

#ifdef _WIN32 //_WIN32 (http_server http protocol issues in linux)
BOOST_AUTO_TEST_CASE(background_mock_azure_get) {
  //start a http server that will receive events sent from the eventhub_client
  http_helper http_server;
  http_server.on_initialize(U("http://localhost:8080"));
  //create a simple ds configuration
  u::config_collection cc;
  auto scode = cfg::create_from_json(JSON_CFG,cc);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  cc.set(r::name::EH_TEST, "true"); // local test event hub
  cc.set("ModelExportFrequency", "00:01:00");

  m::i_data_transport* temp_transport = nullptr;
  scode = r::data_transport_factory.create(&temp_transport, r::value::AZURE_STORAGE_BLOB, cc);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  std::unique_ptr<m::i_data_transport> transport(temp_transport);

  r::api_status status;
 
  int repeatms;
  get_export_frequency(cc, repeatms, &status);
  
  int err_ctxt;
  int data_ctxt;
  r::error_callback_fn efn(dummy_error_fn,&err_ctxt);
  m::data_callback_fn dfn(dummy_data_fn, &data_ctxt);

  u::periodic_background_proc<m::model_downloader> bgproc(repeatms,&efn);

  const auto start = std::chrono::system_clock::now();
  m::model_downloader md(transport.get(), &dfn);
  scode = bgproc.init(&md);
  bgproc.stop();
  const auto stop = std::chrono::system_clock::now();
  const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>( stop - start );
  BOOST_CHECK_LE(diff.count(), 500);
  BOOST_CHECK_EQUAL(data_ctxt, 20);

  http_server.on_shutdown();
}

BOOST_AUTO_TEST_CASE(mock_azure_storage_model_data)
{ 
  //start a http server that will receive events sent from the eventhub_client
  http_helper http_server;
  BOOST_CHECK(http_server.on_initialize(U("http://localhost:8080")));
  //create a simple ds configuration
  u::config_collection cc;
  auto scode = cfg::create_from_json(JSON_CFG,cc);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  cc.set(r::name::EH_TEST, "true"); // local test event hub

  m::i_data_transport* data_transport;
  r::api_status status;
  scode = r::data_transport_factory.create(&data_transport, r::value::AZURE_STORAGE_BLOB, cc, &status);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  m::model_data md;
  BOOST_CHECK_EQUAL(md.refresh_count(), 0);
  scode = data_transport->get_data(md, &status);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  BOOST_CHECK_EQUAL(md.refresh_count(), 1);
  scode = data_transport->get_data(md, &status);
  BOOST_CHECK_EQUAL(md.refresh_count(), 2);

  delete data_transport;

  http_server.on_shutdown();
=======
    if ( interval_ms == 0 )
      RETURN_ERROR(status, e::bad_time_interval, e::bad_time_interval_s);
    return e::success;
  }
  else {
    RETURN_ERROR(status, e::bad_time_interval, e::bad_time_interval_s);
  }
}

void dummy_error_fn(const r::api_status& err, void* ctxt) {
  *( (int*)ctxt ) = 10;
}

void dummy_data_fn(const m::model_data& data, void* ctxt) {
  *( (int*)ctxt ) = 20;
}

BOOST_AUTO_TEST_CASE(background_azure_get) {
  u::config_collection cc;
  cc.set(r::name::MODEL_BLOB_URI, A_BLOB_URI);
  cc.set("ModelExportFrequency", "00:01:00");

  m::i_data_transport* data_transport;
  auto scode = r::data_transport_factory.create(&data_transport, r::value::AZURE_STORAGE_BLOB, cc);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);

  r::api_status status;
 
  int repeatms;
  get_export_frequency(cc, repeatms, &status);
  
  int err_ctxt;
  int data_ctxt;
  r::error_callback_fn efn(dummy_error_fn,&err_ctxt);
  m::data_callback_fn dfn(dummy_data_fn, &data_ctxt);

  m::model_downloader bg(data_transport,&dfn);
  u::periodic_background_proc<m::model_downloader> bgproc(repeatms,bg,&efn);

  const auto start = std::chrono::system_clock::now();
  scode = bgproc.init();
  bgproc.stop();
  const auto stop = std::chrono::system_clock::now();
  const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>( stop - start );
  BOOST_CHECK_LE(diff.count(), 500);
  BOOST_CHECK_EQUAL(data_ctxt, 20);
}

BOOST_AUTO_TEST_CASE(azure_storage_model_data)
{
  u::config_collection cc;
  cc.set(r::name::MODEL_BLOB_URI,  A_BLOB_URI);

  m::i_data_transport* data_transport;
  r::api_status status;
  auto scode = r::data_transport_factory.create(&data_transport, r::value::AZURE_STORAGE_BLOB, cc, &status);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  m::model_data md;
  BOOST_CHECK_EQUAL(md.data_refresh_count, 0);
  scode = data_transport->get_data(md, &status);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  BOOST_CHECK_EQUAL(md.data_refresh_count, 1);
  scode = data_transport->get_data(md, &status);
  BOOST_CHECK_EQUAL(md.data_refresh_count, 1);

  delete data_transport;
>>>>>>> master
}
#endif //_WIN32 (http_server http protocol issues in linux)

void register_local_file_factory();
const char * const DUMMY_DATA_TRANSPORT = "DUMMY_DATA_TRANSPORT";
const char * const CFG_PARAM = "model.local.file";

BOOST_AUTO_TEST_CASE(data_transport_user_extention)
{
  register_local_file_factory();
<<<<<<< HEAD
  const u::config_collection cc;
=======

  u::config_collection cc;
  cc.set(CFG_PARAM,  CFG_VALUE);
>>>>>>> master

  m::i_data_transport* data_transport;
  auto scode = r::data_transport_factory.create(&data_transport, DUMMY_DATA_TRANSPORT, cc);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  m::model_data md;
  scode = data_transport->get_data(md);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
<<<<<<< HEAD
  md.free();
=======
>>>>>>> master
  delete data_transport;
}

BOOST_AUTO_TEST_CASE(vw_model_factory)
{
  register_local_file_factory();
<<<<<<< HEAD
  
  u::config_collection model_cc;
  model_cc.set(r::name::VW_CMDLINE, "--lda 5");
  m::i_model* vw;
  const auto scode = r::model_factory.create(&vw, r::value::VW, model_cc);
=======
  auto model = get_model_data();

  u::config_collection model_cc;
  model_cc.set(r::name::VW_CMDLINE, "--lda 5");
  m::i_model* vw;
  auto scode = r::model_factory.create(&vw, r::value::VW, model_cc);
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  vw->init(model);

  char* features = "1 2 3";
  int actions[] = { 1,2,3 };
  scode = vw->choose_rank(features, actions);
>>>>>>> master
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  delete vw;
}

m::model_data get_model_data()
{
<<<<<<< HEAD
  const u::config_collection cc;
=======
  u::config_collection cc;
  cc.set(CFG_PARAM, CFG_VALUE);

>>>>>>> master
  m::i_data_transport* data_transport;
  r::data_transport_factory.create(&data_transport, DUMMY_DATA_TRANSPORT, cc);
  std::unique_ptr<m::i_data_transport> pdt(data_transport);
  m::model_data md;
<<<<<<< HEAD
  const auto scode = pdt->get_data(md);
=======
  auto scode = pdt->get_data(md);
>>>>>>> master
  BOOST_CHECK_EQUAL(scode, r::error_code::success);
  return md;
}

class dummy_data_transport : public m::i_data_transport {
<<<<<<< HEAD
  int get_data(m::model_data& data, r::api_status* status) override {
    data.alloc(10);
=======
  virtual int get_data(m::model_data& data, r::api_status* status = nullptr) {
    data.data = nullptr;
    data.data_sz = 0;
>>>>>>> master
    return r::error_code::success;
  }
};

int dummy_data_tranport_create( m::i_data_transport** retval, 
                                    const u::config_collection& cfg, 
                                    r::api_status* status) {
  *retval = new dummy_data_transport();
  return r::error_code::success;
}

void register_local_file_factory() {
  r::data_transport_factory.register_type(DUMMY_DATA_TRANSPORT, dummy_data_tranport_create);
}
