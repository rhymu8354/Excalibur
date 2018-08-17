import React, { Component } from 'react';
import { AutoSizer, List } from 'react-virtualized';
import './App.css';

/**
 * This function adds the given element to the list, if it isn't
 * already in the list.
 * @param {*} list This is the list to which to add the element.
 * @param {*} elementToAdd This is the element to add to the list.
 */
function addToList(list, elementToAdd) {
    return [
        elementToAdd,
        ...list.filter((listElement) => listElement != elementToAdd)
    ];
}

class App extends Component {
    constructor(props) {
        super(props);
        this.state = {
            serverUrl: `${window.location.hostname}/chat`,
            socketStatus: "disconnected",

            /**
             * This is the nickname accepted by the server.
             */
            currentNickname: '',

            /**
             * This is the nickname that is being set in the server.
             */
            nicknameToSet: '',

            /**
             * These are the nicknames the server has provided
             * as available for use.
             */
            availableNicknames: [],

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

    changeNewTell = (event) => {
        this.setState({
            newTell: event.target.value
        });
    }

    changeNickname = (event) => {
        if (this.socket) {
            this.onSetNickname(event.target.value);
        } else {
            this.setState({
                currentNickname: event.target.value
            });
        }
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
        const lastNickname = this.state.currentNickname;
        this.setState({
            socketStatus: "CONNECTED",
            currentNickname: "",
        });
        if (lastNickname !== '') {
            this.onSetNickname(lastNickname);
        }
        this.socket.send(JSON.stringify({Type: "GetUsers"}));
        this.socket.send(JSON.stringify({Type: "GetAvailableNickNames"}));
    }

    onMessageReceived = (message) => {
        switch (message.Type) {
            case 'SetNickNameResult': {
                if (message.Success) {
                    const currentNickname = this.state.nicknameToSet;
                    this.setState({
                        currentNickname: currentNickname,
                        availableNicknames: addToList(this.state.availableNicknames, currentNickname),
                    });
                }
            } break;

            case 'Users': {
                this.setState({
                    users: message.Users,
                });
            } break;

            case 'AvailableNickNames': {
                this.setState({
                    availableNicknames: addToList(addToList(message.AvailableNickNames, this.state.currentNickname), ""),
                });
            } break;

            case 'Join': {
                const user = {
                    "Nickname": message.NickName,
                    "Points": 0,
                };
                this.setState({
                    users: [...this.state.users, user],
                    availableNicknames: this.state.availableNicknames.filter(
                        (nickname) => (
                            (nickname == this.state.currentNickname)
                            || (nickname !== message.NickName)
                            || (nickname === "")
                        )
                    ),
                });
            } break;

            case 'Leave': {
                this.setState({
                    users: this.state.users.filter(
                        user => user.Nickname !== message.NickName
                    ),
                    availableNicknames: addToList(this.state.availableNicknames, message.NickName),
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
            });
            this.socket.close();
            this.socket = null;
        }
    }

    onSetNickname = (nicknameToSet) => {
        if (this.socket) {
            this.setState({
                nicknameToSet: nicknameToSet,
                currentNickname: "",
            });
            const message = {
                Type: "SetNickName",
                NickName: nicknameToSet,
            };
            this.socket.send(JSON.stringify(message));
        }
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
        const availableNicknames = this.state.availableNicknames.sort(
            (a, b) => {
                if (a < b) return -1;
                else if (a > b) return 1;
                else return 0;
            }
        );
        const nicknameSelectOptions = this.state.availableNicknames.map((availableNickname) => {
            return <option key={availableNickname} value={availableNickname}>{availableNickname}</option>;
        });
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
                            <select
                                className="App-Nickname-input"
                                value={this.state.currentNickname}
                                onChange={this.changeNickname}
                            >
                                {nicknameSelectOptions}
                            </select>
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
