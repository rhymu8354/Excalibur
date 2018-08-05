import React, { Component } from 'react';
import { AutoSizer, List } from 'react-virtualized';
import './App.css';

class App extends Component {
    constructor(props) {
        super(props);
        this.state = {
            serverUrl: 'localhost:8080/chat',
            socketStatus: "disconnected",

            /**
             * This is the nickname accepted by the server.
             */
            currentNickname: '',

            /**
             * This is the nickname being typed by the user.
             */
            newNickname: '',

            /**
             * This is the nickname that is being set in the server.
             */
            nicknameToSet: '',

            /**
             * These are the nicknames of the users currently
             * in the chat room.
             */
            nicknames: [],

            /**
             * This is the text the user will send as a tell if
             * they click the 'send' button.
             */
            newTell: '',

            /**
             * This holds the sequence of tells received from the room.
             */
            tells: [],

            /**
             * This is the next ID key to assign for the next tell received.
             */
            nextTellId: 1,
        };
        this.socket = null;
    }

    changeServerUrl = (event) => {
        this.setState({
            serverUrl: event.target.value
        });
    }

    changeNewNickname = (event) => {
        this.setState({
            newNickname: event.target.value
        });
    }

    changeNewTell = (event) => {
        this.setState({
            newTell: event.target.value
        });
    }

    onConnect = () => {
        this.socket = new WebSocket('ws://' + this.state.serverUrl);
        this.socket.addEventListener(
            'open',
            (event) => { this.onConnected(); }
        );
        this.socket.addEventListener(
            'message',
            (event) => { this.onMessageReceived(JSON.parse(event.data)); }
        );
    }

    onConnected = () => {
        this.setState({
            socketStatus: "CONNECTED",
        });
        if (this.state.newNickname !== '') {
            this.onSetNickname();
        }
        this.socket.send(JSON.stringify({Type: "GetNickNames"}));
    }

    onMessageReceived = (message) => {
        switch (message.Type) {
            case 'SetNickNameResult':
                if (message.Success) {
                    this.setState({
                        currentNickname: this.state.nicknameToSet,
                    });
                }
                break;

            case 'NickNames':
                this.setState({
                    nicknames: message.NickNames,
                });
                break;

            case 'Join':
                this.setState({
                    nicknames: [...this.state.nicknames, message.NickName],
                });
                break;

            case 'Leave':
                this.setState({
                    nicknames: this.state.nicknames.filter(
                        nickname => nickname !== message.NickName
                    ),
                });
                break;

            case 'Tell':
                const tell = {
                    id: this.state.nextTellId,
                    sender: message.Sender,
                    tell: message.Tell,
                };
                this.setState({
                    tells: [...this.state.tells, tell],
                    nextTellId: this.state.nextTellId + 1,
                });
                break;

            default:
                break;
        }
    }

    onDisconnect = () => {
        this.setState({
            socketStatus: "disconnected",
            nicknames: [],
            currentNickname: '',
        });
        this.socket.close();
    }

    onSetNickname = () => {
        if (this.socket) {
            this.setState({
                nicknameToSet: this.state.newNickname,
            });
            const message = {
                Type: "SetNickName",
                NickName: this.state.newNickname,
                Password: "SeemsGood",
            };
            this.socket.send(JSON.stringify(message));
        }
    }

    onResetNickname = () => {
        this.setState({
            newNickname: this.state.nicknameToSet,
        });
    }

    onSendTell = () => {
        const message = {
            Type: "Tell",
            Tell: this.state.newTell
        };
        this.socket.send(JSON.stringify(message));
        this.setState({
            newTell: '',
        });
    }

    renderTell = ({
        style,
        key,
        index,
    }) => {
        const tell = this.state.tells[index];
        return (
            <div style={style} key={key}>
                {tell.sender}: {tell.tell}
            </div>
        );
    }

    render() {
        const nicknames = this.state.nicknames.map((nickname) =>
            <li key={nickname}>
                {nickname}
            </li>
        );
        const tells = (
            <AutoSizer>
                {({ height, width }) => (
                    <List
                        height={height}
                        rowCount={this.state.tells.length}
                        rowHeight={21}
                        rowRenderer={this.renderTell}
                        scrollToIndex={this.state.tells.length - 1}
                        width={width}
                    />
                )}
            </AutoSizer>
        );
        return (
            <div className="App">
                <header className="App-header">
                    <h1 className="App-title">Chatter</h1>
                </header>
                <div className="App-main">
                    <div className="App-main-left">
                        <div>
                            WebSocket status: <span className="App-Socket-status">{this.state.socketStatus}</span>
                        </div>
                        <div>
                            <input
                                value={this.state.serverUrl}
                                onChange={this.changeServerUrl}
                            />
                            <button onClick={this.onConnect} >Connect</button>
                            <button onClick={this.onDisconnect} >Disconnect</button>
                        </div>
                        <div>
                            Nickname:
                            <input
                                className="App-Nickname-input"
                                value={this.state.newNickname}
                                onChange={this.changeNewNickname}
                            />
                            <button onClick={this.onSetNickname}>Set</button>
                            <button onClick={this.onResetNickname}>Reset</button>
                        </div>
                        <div className="App-Tells">
                            {tells}
                        </div>
                        <div className="App-Tell">
                            <input
                                className="App-Tell-input"
                                value={this.state.newTell}
                                onChange={this.changeNewTell}
                            />
                            <button onClick={this.onSendTell}>Send</button>
                        </div>
                    </div>
                    <div className="App-main-right">
                        Users currently here:
                        <ul>{nicknames}</ul>
                    </div>
                </div>
            </div>
        );
    }
}

export default App;
