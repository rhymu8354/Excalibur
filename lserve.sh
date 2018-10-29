ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
$ROOT/build-vscode/WebServer/WebServer -c $ROOT/config.json
