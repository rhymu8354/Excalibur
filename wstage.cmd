@set ROOT=%~dp0
@set PLUGINS=%ROOT%build\WebServer\Debug\plugins
@set RUNTIME=%PLUGINS%\runtime
@if not exist %PLUGINS% mkdir %PLUGINS%
@if not exist %RUNTIME% mkdir %RUNTIME%
copy %ROOT%build\WebServer\StaticContentPlugin\Debug\StaticContentPlugin.dll %PLUGINS%
copy %ROOT%build\WebServer\ChatRoomPlugin\Debug\ChatRoomPlugin.dll %PLUGINS%
