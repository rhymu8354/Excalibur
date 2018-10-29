@set ROOT=%~dp0
@set STAGE=%ROOT%build-vscode\WebServer
@set PLUGINS=%STAGE%\plugins
@set RUNTIME=%PLUGINS%\runtime
@if not exist %PLUGINS% mkdir %PLUGINS%
@if not exist %RUNTIME% mkdir %RUNTIME%
copy %ROOT%build-vscode\WebServer\ChatRoomPlugin\ChatRoomPlugin.dll %PLUGINS%
copy %ROOT%build-vscode\WebServer\EchoPlugin\EchoPlugin.dll %PLUGINS%
copy %ROOT%build-vscode\WebServer\StaticContentPlugin\StaticContentPlugin.dll %PLUGINS%
copy %ROOT%test-cert-key-localhost\cert.pem %STAGE%
copy %ROOT%test-cert-key-localhost\key.pem %STAGE%
