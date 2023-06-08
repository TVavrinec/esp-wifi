// WebSockets client
const ws = new WebSocket('ws://' + location.hostname + '/ws');

ws.addEventListener('open', () => {
    console.log('WebSocket connection established');
});

ws.addEventListener('message', (event) => {
    const data = JSON.parse(event.data);

    if (data.type === 'login') {
        if (data.user) {
            ws.close();
            // Go to desired page
            window.location.href = '/'+data.user+'.html';
        } else {
            // Show error message
            console.error('Login failed');
        }
    }
});

document.querySelector('form').addEventListener('submit', (event) => {
    event.preventDefault();

    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    // Send login data via WebSocket
    ws.send(JSON.stringify({ type: 'signin', username, password }));
});