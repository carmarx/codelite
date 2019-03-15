//===--------------------------------------------------------------------------------
// Copyright: Eran Ifrah
// This scripts attempts to launch LSP server(s)
// and pass the communication with them over socket
// to CodeLite
//===--------------------------------------------------------------------------------
let net = require('net');
const { spawn } = require('child_process');

///===-------------------------------------------------------------------------------
/// Message
///===-------------------------------------------------------------------------------
class Message
{
    constructor()
    {
        this.content = "";
        this.headersMap = new Map()
    }

    /**
     * read header from the buffer
     */

    getContentLength()
    {
        if(this.headersMap.has("content-length")) {
           return parseInt(this.headersMap.get("content-length"));
        }
        return -1;
    }

    parse(buffer)
    {
        let str = (typeof buffer == "string") ? buffer : buffer.toString();
        let where = str.indexOf("\r\n\r\n");
        if(where != -1) {
            this.content = str.substr(where + 4);
            let headers = str.substr(0, where).split("\r\n");
            this.headersMap.clear();
            headers.forEach(function(v) {
                let pair = v.split(":");
                if(pair.length == 2) { 
                    let key = pair[0].trim().toLowerCase();
                    let val = pair[1].trim().toLowerCase();
                    this.headersMap.set(key, val);
                }
            }.bind(this));
        }
        let contentLen = this.getContentLength();
        if((contentLen == -1) || (contentLen > this.content.length)) { return undefined; }
        let messageBuffer = this.content.substr(0, contentLen);
        let remainder = this.content.substr(contentLen);
        
        try {
            let requestObject = JSON.parse(messageBuffer);
            return { request: requestObject, remainder: remainder };
        } catch (e) {
            return undefined;
        }
    }
}

///===---------------------------------------------------------------------------------------------
///===---------------------------------------------------------------------------------------------
///===---------------------------------------------------------------------------------------------

function createTCPServer()
{
    let server = net.createServer();
    // Make the server TCP/IP server on port 12898
    let port = (process.argv.length > 2) ? process.argv[2] : 12898;
    server.listen(port, '127.0.0.1');
    return server;
}

/**
 * start LSP server and associate it with the TCP connection
 */
function processCommand(command, conn)
{
    switch(command.method) {
    case 'execute':
        // The 'execute' is a special command that we handle it ourself
        let lsp = spawn(command.command);
        if(lsp != undefined) {
            conn.lsp_process = lsp;
            lsp.stdout.on('data', (data) => {
                console.log("LSP\n" + data.toString());
                conn.write(data.toString()); // pass the data as-is back to CodeLite
            });
            lsp.stderr.on('stderr', (data) => {
                console.error("LSP\n" + data.toString()); 
            });
            lsp.on('close', (code) => {
               // Close the tcp connection
               process.exit(0);
            });
        }
    break;
    default:
        if(conn.lsp_process != undefined) {
            let asString = JSON.stringify(command);
            let jsonMessage = "Content-Length: " + asString.length + "\r\n\r\n" + asString; 
            conn.lsp_process.stdin.write(jsonMessage);
        }
    break;
    }
}

/**
 * this callback will get called when data arrives on the socket
 * @param data string|buffer
 * @this the connection (socket)
 */
function onDataRead(data)
{
    if(!this.hasOwnProperty('incoming_buffer')) { this['incoming_buffer'] = ""; }
    this.incoming_buffer += typeof data == "string" ? data : data.toString();
    while(true) {
        let message = new Message();
        let command = message.parse(this.incoming_buffer);
        if(command == undefined) { break; }
        this.incoming_buffer = command.remainder;
        processCommand(command.request, this);
    }
}

//===--------------------------------------------------------------------------------
// Main
//===--------------------------------------------------------------------------------

// Start a server
let server = createTCPServer();
server.on('connection', (conn) => {
    console.log("Client connected!");
    conn.on('data', onDataRead.bind(conn));
    conn.on('close', (code) => {
        // if CodeLite termianted, exit
        process.exit(0);
    });
});