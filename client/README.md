# Chat API — Protocol Reference

All messages are transmitted over a persistent TCP connection as **newline-delimited compact JSON** (`\n` terminates each frame).

---

## Message Format

Every message sent from the client includes a `type` field that identifies the operation.

```
{"type":"<TYPE>", ...fields}\n
```

---

## Endpoints

### AUTH — Authenticate

Sent immediately after the TCP connection is established. Registers the client with the server under a given username.

**Direction:** Client → Server

**Request**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | ✅ | Must be `"AUTH"` |
| `name` | string | ✅ | The username to register |

```json
{"type":"AUTH","name":"alice"}
```

---

### MSG — Send a Message

Sends a chat message to a specific room.

**Direction:** Client → Server

**Request**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | ✅ | Must be `"MSG"` |
| `name` | string | ✅ | The sender's username |
| `message` | string | ✅ | The message body |
| `roomid` | integer | ✅ | ID of the target room |

```json
{"type":"MSG","name":"alice","message":"Hello!","roomid":42}
```

---

### EXIT — Disconnect

Notifies the server that the client is disconnecting gracefully.

**Direction:** Client → Server

**Request**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | ✅ | Must be `"EXIT"` |
| `name` | string | ✅ | The username that is leaving |

```json
{"type":"EXIT","name":"alice"}
```

---

### CHATCRT — Create a Chat Room

Requests the creation of a new chat room and adds a set of users to it by their IDs.

**Direction:** Client → Server

**Request**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type` | string | ✅ | Must be `"CHATCRT"` |
| `chatname` | string | ✅ | Display name for the new room |
| `ids` | integer[] | ✅ | List of user IDs to add to the room |

```json
{"type":"CHATCRT","chatname":"dev-team","ids":[1,2,3]}
```

---

## Summary

| Type | Direction | Purpose |
|------|-----------|---------|
| `AUTH` | Client → Server | Authenticate and announce presence |
| `MSG` | Client → Server | Send a chat message to a room |
| `EXIT` | Client → Server | Graceful disconnect notification |
| `CHATCRT` | Client → Server | Create a new chat room with members |
