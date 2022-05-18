// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/electron_api_push_notifications.h"

#include "shell/browser/browser.h"
#include "shell/common/gin_helper/dictionary.h"
#include "shell/common/gin_helper/object_template_builder.h"
#include "shell/common/node_includes.h"

namespace electron {

namespace api {

PushNotifications::PushNotifications() {
#if BUILDFLAG(IS_MAC)
  static_cast<ElectronBrowserClient*>(ElectronBrowserClient::Get())
      ->set_delegate(this);
  Browser::Get()->AddObserver(this);
#endif
}

PushNotifications::~PushNotifications() {
#if BUILDFLAG(IS_MAC)
  static_cast<ElectronBrowserClient*>(ElectronBrowserClient::Get())
      ->set_delegate(nullptr);
  Browser::Get()->RemoveObserver(this);
#endif
}

#if BUILDFLAG(IS_MAC)

void PushNotifications::OnDidRegisterForAPNSNotificationsWithDeviceToken(
    const std::string& token) {
  Emit("registered-for-apns-notifications", token);
}

void PushNotifications::OnDidFailToRegisterForAPNSNotificationsWithError(
    const std::string& error) {
  Emit("failed-to-register-for-apns-notifications", error);
}

void PushNotifications::OnDidReceiveAPNSNotification(
    const base::DictionaryValue& user_info) {
  Emit("received-apns-notification", user_info);
}
#endif

// static
gin::ObjectTemplateBuilder PushNotifications::GetObjectTemplateBuilder(
    v8::Isolate* isolate) {
  return gin_helper::EventEmitterMixin<PushNotifications>::GetObjectTemplateBuilder(
             isolate)
#if BUILDFLAG(IS_MAC)
      .SetMethod("registerForAPNSNotifications",
                 base::BindRepeating(&Browser::RegisterForAPNSNotifications,
                                     browser))
      .SetMethod("unregisterForAPNSNotifications",
                 base::BindRepeating(&Browser::UnregisterForAPNSNotifications,
                                     browser))
#endif

}

}  // namespace api

}  // namespace electron


namespace {

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  gin::Dictionary dict(isolate, exports);
  dict.Set("pushNotifications", electron::api::PushNotifications::Create(isolate));
}

}  // namespace


NODE_LINKED_MODULE_CONTEXT_AWARE(electron_browser_push_notifications, Initialize)