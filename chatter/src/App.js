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
             * These are the users currently in the chat room.
             */
            users: [],

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
        if (this.socket) {
            return;
        }
        this.socket = new WebSocket('ws://' + this.state.serverUrl);
        this.socket.addEventListener(
            'open',
            (event) => { this.onConnected(); }
        );
        this.socket.addEventListener(
            'close',
            (event) => { this.onDisconnect(); }
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
        this.socket.send(JSON.stringify({Type: "GetUsers"}));
    }

    onMessageReceived = (message) => {
        switch (message.Type) {
            case 'SetNickNameResult': {
                if (message.Success) {
                    this.setState({
                        currentNickname: this.state.nicknameToSet,
                    });
                }
            } break;

            case 'Users': {
                this.setState({
                    users: message.Users,
                });
            } break;

            case 'Join': {
                const user = {
                    "Nickname": message.NickName,
                    "Points": 0,
                };
                this.setState({
                    users: [...this.state.users, user],
                });
            } break;

            case 'Leave': {
                this.setState({
                    users: this.state.users.filter(
                        user => user.Nickname !== message.NickName
                    ),
                });
            } break;

            case 'Tell': {
                const tell = {
                    id: this.state.nextTellId,
                    sender: message.Sender,
                    tell: message.Tell,
                };
                this.setState({
                    tells: [...this.state.tells, tell],
                    nextTellId: this.state.nextTellId + 1,
                });
            } break;

            case 'Award': {
                const tell = {
                    id: this.state.nextTellId,
                    sender: null,
                    color: "App-Tell-award",
                    tell: `Congratulations, ${message.Subject}!  That's the correct answer.`,
                };
                let subject = this.state.users.find(user => user.Nickname === message.Subject);
                ++subject.Points;
                this.setState({
                    tells: [...this.state.tells, tell],
                    nextTellId: this.state.nextTellId + 1,
                    users: [
                        ...this.state.users.filter(
                            user => user.Nickname !== message.Subject
                        ),
                        subject
                    ],
                });
            } break;

            case 'Penalty': {
                const tell = {
                    id: this.state.nextTellId,
                    sender: null,
                    color: "App-Tell-penalty",
                    tell: `Sorry, ${message.Subject}, that isn't the correct answer.`,
                };
                let subject = this.state.users.find(user => user.Nickname === message.Subject);
                --subject.Points;
                this.setState({
                    tells: [...this.state.tells, tell],
                    nextTellId: this.state.nextTellId + 1,
                    users: [
                        ...this.state.users.filter(
                            user => user.Nickname !== message.Subject
                        ),
                        subject
                    ],
                });
            } break;

            default:
                break;
        }
    }

    onDisconnect = () => {
        if (this.socket) {
            this.setState({
                socketStatus: "disconnected",
                users: [],
                currentNickname: '',
            });
            this.socket.close();
            this.socket = null;
        }
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
        if (this.socket) {
            const message = {
                Type: "Tell",
                Tell: this.state.newTell
            };
            this.socket.send(JSON.stringify(message));
            this.setState({
                newTell: '',
            });
        }
    }

    renderTell = ({
        style,
        key,
        index,
    }) => {
        const tell = this.state.tells[index];
        if (tell.sender) {
            return (
                <div style={style} key={key}>
                    {tell.sender}: {tell.tell}
                </div>
            );
        } else {
            return (
                <div style={style} className={tell.color} key={key}>
                    {tell.tell}
                </div>
            );
        }
    }

    render() {
        const users = this.state.users.sort((a, b) => b.Points - a.Points);
        const nicknames = users.map((user) =>
            <tr key={user.Nickname}>
                <td>{user.Nickname}</td>
                <td>{user.Points}</td>
            </tr>
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
                        <table>
                            <thead>
                                <tr>
                                    <th>Nickname</th>
                                    <th>Points</th>
                                </tr>
                            </thead>
                            <tbody>
                                {nicknames}
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>
        );
    }
}

export default App;
