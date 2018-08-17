ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
PLUGINS=$ROOT/build/WebServer/plugins
RUNTIME=$PLUGINS/runtime
mkdir -p $PLUGINS
mkdir -p $RUNTIME
cp $ROOT/build/WebServer/StaticContentPlugin/libStaticContentPlugin.so $PLUGINS
cp $ROOT/build/WebServer/ChatRoomPlugin/libChatRoomPlugin.so $PLUGINS
