@set ROOT=%~dp0
@set STAGE=%ROOT%build\WebServer\Debug
@set PLUGINS=%STAGE%\plugins
@set RUNTIME=%PLUGINS%\runtime
@if not exist %PLUGINS% mkdir %PLUGINS%
@if not exist %RUNTIME% mkdir %RUNTIME%
copy %ROOT%build\WebServer\ChatRoomPlugin\Debug\ChatRoomPlugin.dll %PLUGINS%
copy %ROOT%build\WebServer\EchoPlugin\Debug\EchoPlugin.dll %PLUGINS%
copy %ROOT%build\WebServer\StaticContentPlugin\Debug\StaticContentPlugin.dll %PLUGINS%
copy %ROOT%test-cert-key-with-password\cert.pem %STAGE%
copy %ROOT%test-cert-key-with-password\key.pem %STAGE%
