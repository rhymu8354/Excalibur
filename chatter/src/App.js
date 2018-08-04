import React, { Component } from 'react';
import './App.css';

class App extends Component {
    constructor(props) {
        super(props);
        this.state = {
            serverUrl: 'localhost:8080/chat',
            socketStatus: "disconnected",
        };
        this.socket = null;
    }

    changeServerUrl = (event) => {
        this.setState({
            serverUrl: event.target.value
        });
    }

    onConnect = () => {
        this.socket = new WebSocket('ws://' + this.state.serverUrl);
        this.socket.addEventListener(
            'open',
            (event) => { this.onConnected(); }
        );
    }

    onConnected = (event) => {
        this.setState({
            socketStatus: "CONNECTED",
        });
    }

    onDisconnect = () => {
        this.setState({
            socketStatus: "disconnected",
        });
        this.socket.close();
    }

    render() {
        return (
            <div className="App">
                <header className="App-header">
                    <h1 className="App-title">Welcome to React</h1>
                </header>
                <div className="App-main">
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
                </div>
            </div>
        );
    }
}

export default App;
