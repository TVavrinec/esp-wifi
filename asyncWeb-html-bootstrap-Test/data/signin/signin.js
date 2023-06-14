// // WebSockets client
// const ws = new WebSocket('ws://' + location.hostname + '/ws');

// ws.addEventListener('open', () => {
//     console.log('WebSocket connection established');
// });

// ws.addEventListener('message', (event) => {
//     const data = JSON.parse(event.data);

//     if (data.type === 'login') {
//         if (data.user) {
//             ws.close();
//             // Go to desired page
//             fetch('/'+data.user+'.html', {
//                 headers: {
//                     'Session-ID': 'your-session-id-goes-here',
//                 },
//             })            
//             window.location.href = '/'+data.user+'.html';
//         } else {
//             // Show error message
//             console.error('Login failed');
//         }
//     }
// });


document.querySelector('form').addEventListener('submit', function(event) {
    event.preventDefault();

    var username = document.getElementById('username').value;
    var password = document.getElementById('password').value;

    fetch('/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({
            username: username,
            password: password
        }),
    })
    .then(response => {
        console.log('Response: ', response);
        return response.json()
    })
    .then(data => {
        console.log('Success: ', data);
        if (data.user) {
            // Přihlášení bylo úspěšné, přesměrujeme uživatele na hlavní stránku
            window.location.href = '/'+data.user+'.html';
        } else {
            // Přihlášení se nezdařilo, zobrazíme chybovou zprávu
            alert('Login failed!');
        }
    })
    .catch((error) => {
        console.error('Error:', error);
    });
});
