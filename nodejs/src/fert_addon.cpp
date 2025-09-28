#include <node_api.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include "fert.hpp"

// Helper macros for error handling
#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;         \
      napi_get_last_error_info((env), &error_info);              \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);             \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
          ? "empty error message"                                 \
          : error_info->error_message;                            \
        napi_throw_error((env), NULL, message);                   \
      }                                                           \
      return NULL;                                                \
    }                                                             \
  } while(0)

struct FertAddon {
  std::unique_ptr<fert::Cfert> fert_;
  napi_ref wrapper_;
};

// Destructor callback
static void FertDestructor(napi_env env, void* nativeObject, void* /*finalize_hint*/) {
  FertAddon* addon = static_cast<FertAddon*>(nativeObject);
  napi_delete_reference(env, addon->wrapper_);
  delete addon;
}

// Constructor
static napi_value FertConstructor(napi_env env, napi_callback_info info) {
  napi_value target;
  NAPI_CALL(env, napi_get_new_target(env, info, &target));
  bool is_constructor = target != nullptr;
  
  if (is_constructor) {
    // Called as constructor: new Fert(...)
    size_t argc = 1;
    napi_value args[1];
    napi_value jsthis;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr));
    
    if (argc < 1) {
      napi_throw_error(env, nullptr, "Expected metakernel path argument");
      return nullptr;
    }
    
    // Get the metakernel path
    size_t path_length;
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], nullptr, 0, &path_length));
    std::string metakernel_path(path_length, '\0');
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], &metakernel_path[0], path_length + 1, nullptr));
    
    // Create the addon instance
    FertAddon* addon = new FertAddon();
    try {
      addon->fert_ = std::make_unique<fert::Cfert>(metakernel_path.c_str());
    } catch (const std::exception& e) {
      delete addon;
      napi_throw_error(env, nullptr, ("Failed to initialize FERT: " + std::string(e.what())).c_str());
      return nullptr;
    }
    
    // Wrap the addon instance
    NAPI_CALL(env, napi_wrap(env, jsthis, addon, FertDestructor, nullptr, &addon->wrapper_));
    
    return jsthis;
  } else {
    // Called as function: Fert(...)
    napi_throw_error(env, nullptr, "Fert must be called as constructor");
    return nullptr;
  }
}

// Helper function to get addon from wrapped object
static FertAddon* GetAddon(napi_env env, napi_value jsthis) {
  FertAddon* addon;
  napi_status status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&addon));
  if (status != napi_ok) return nullptr;
  return addon;
}

// Get state (position only)
static napi_value GetState(napi_env env, napi_callback_info info) {
  size_t argc = 4;
  napi_value args[4];
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr));
  
  if (argc < 4) {
    napi_throw_error(env, nullptr, "Expected 4 arguments: et, targetID, centerID, referenceID");
    return nullptr;
  }
  
  FertAddon* addon = GetAddon(env, jsthis);
  if (!addon) {
    napi_throw_error(env, nullptr, "Invalid FERT instance");
    return nullptr;
  }
  
  double et;
  int32_t targetID, centerID, referenceID;
  
  NAPI_CALL(env, napi_get_value_double(env, args[0], &et));
  NAPI_CALL(env, napi_get_value_int32(env, args[1], &targetID));
  NAPI_CALL(env, napi_get_value_int32(env, args[2], &centerID));
  NAPI_CALL(env, napi_get_value_int32(env, args[3], &referenceID));
  
  try {
    double r[3];
    addon->fert_->getState(et, targetID, centerID, referenceID, r);
    
    napi_value result;
    NAPI_CALL(env, napi_create_array_with_length(env, 3, &result));
    
    for (int i = 0; i < 3; i++) {
      napi_value num;
      NAPI_CALL(env, napi_create_double(env, r[i], &num));
      NAPI_CALL(env, napi_set_element(env, result, i, num));
    }
    
    return result;
  } catch (const std::exception& e) {
    napi_throw_error(env, nullptr, ("Error getting state: " + std::string(e.what())).c_str());
    return nullptr;
  }
}

// Get state with velocity
static napi_value GetStateWithVelocity(napi_env env, napi_callback_info info) {
  size_t argc = 4;
  napi_value args[4];
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr));
  
  if (argc < 4) {
    napi_throw_error(env, nullptr, "Expected 4 arguments: et, targetID, centerID, referenceID");
    return nullptr;
  }
  
  FertAddon* addon = GetAddon(env, jsthis);
  if (!addon) {
    napi_throw_error(env, nullptr, "Invalid FERT instance");
    return nullptr;
  }
  
  double et;
  int32_t targetID, centerID, referenceID;
  
  NAPI_CALL(env, napi_get_value_double(env, args[0], &et));
  NAPI_CALL(env, napi_get_value_int32(env, args[1], &targetID));
  NAPI_CALL(env, napi_get_value_int32(env, args[2], &centerID));
  NAPI_CALL(env, napi_get_value_int32(env, args[3], &referenceID));
  
  try {
    double r[3], v[3];
    addon->fert_->getState(et, targetID, centerID, referenceID, r, v);
    
    napi_value result;
    NAPI_CALL(env, napi_create_object(env, &result));
    
    // Create position array
    napi_value position;
    NAPI_CALL(env, napi_create_array_with_length(env, 3, &position));
    for (int i = 0; i < 3; i++) {
      napi_value num;
      NAPI_CALL(env, napi_create_double(env, r[i], &num));
      NAPI_CALL(env, napi_set_element(env, position, i, num));
    }
    
    // Create velocity array
    napi_value velocity;
    NAPI_CALL(env, napi_create_array_with_length(env, 3, &velocity));
    for (int i = 0; i < 3; i++) {
      napi_value num;
      NAPI_CALL(env, napi_create_double(env, v[i], &num));
      NAPI_CALL(env, napi_set_element(env, velocity, i, num));
    }
    
    NAPI_CALL(env, napi_set_named_property(env, result, "position", position));
    NAPI_CALL(env, napi_set_named_property(env, result, "velocity", velocity));
    
    return result;
  } catch (const std::exception& e) {
    napi_throw_error(env, nullptr, ("Error getting state with velocity: " + std::string(e.what())).c_str());
    return nullptr;
  }
}

// Print SPK summary
static napi_value PrintSpkSummary(napi_env env, napi_callback_info info) {
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
  
  FertAddon* addon = GetAddon(env, jsthis);
  if (!addon) {
    napi_throw_error(env, nullptr, "Invalid FERT instance");
    return nullptr;
  }
  
  addon->fert_->printSpkSummary();
  
  napi_value undefined;
  NAPI_CALL(env, napi_get_undefined(env, &undefined));
  return undefined;
}

// Print PCK summary
static napi_value PrintPckSummary(napi_env env, napi_callback_info info) {
  napi_value jsthis;
  NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));
  
  FertAddon* addon = GetAddon(env, jsthis);
  if (!addon) {
    napi_throw_error(env, nullptr, "Invalid FERT instance");
    return nullptr;
  }
  
  addon->fert_->printPckSummary();
  
  napi_value undefined;
  NAPI_CALL(env, napi_get_undefined(env, &undefined));
  return undefined;
}

// Initialize module
static napi_value Init(napi_env env, napi_value exports) {
  napi_value fertConstructor;
  
  napi_property_descriptor properties[] = {
    { "getState", 0, GetState, 0, 0, 0, napi_default, 0 },
    { "getStateWithVelocity", 0, GetStateWithVelocity, 0, 0, 0, napi_default, 0 },
    { "printSpkSummary", 0, PrintSpkSummary, 0, 0, 0, napi_default, 0 },
    { "printPckSummary", 0, PrintPckSummary, 0, 0, 0, napi_default, 0 },
  };
  
  NAPI_CALL(env, napi_define_class(env, "Fert", NAPI_AUTO_LENGTH, FertConstructor, nullptr,
    sizeof(properties) / sizeof(properties[0]), properties, &fertConstructor));
  
  NAPI_CALL(env, napi_set_named_property(env, exports, "Fert", fertConstructor));
  
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)