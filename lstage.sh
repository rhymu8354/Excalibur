ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
STAGE=$ROOT/build-vscode/WebServer
PLUGINS=$STAGE/plugins
RUNTIME=$PLUGINS/runtime
mkdir -p $PLUGINS
mkdir -p $RUNTIME
cp $ROOT/build-vscode/WebServer/ChatRoomPlugin/libChatRoomPlugin.so $PLUGINS
cp $ROOT/build-vscode/WebServer/EchoPlugin/libEchoPlugin.so $PLUGINS
cp $ROOT/build-vscode/WebServer/StaticContentPlugin/libStaticContentPlugin.so $PLUGINS
cp $ROOT/test-cert-key-localhost/cert.pem $STAGE
cp $ROOT/test-cert-key-localhost/key.pem $STAGE
