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
