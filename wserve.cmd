@set ROOT=%~dp0
@if "%~1"=="PePe" (
    %ROOT%build\WebServer\Debug\WebServer -c %ROOT%config.json
) else (
    call <nul %0 PePe
)
