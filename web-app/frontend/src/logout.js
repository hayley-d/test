document.getElementById('logoutBTN').addEventListener('click', async () => {

    const token = localStorage.getItem('token');

    if(!token){
        console.error('No token found, please login again.');
        window.location.href = './login.html'; //redirect to login page
        return;
    }

    try{
        const response = await fetch('http://localhost:3000/logout', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ token }),
        });

        if (!response.ok) {
            throw new Error('Logout failed');
        }

        const result = await response.json();
        console.log('Logout successful:', result);

        // Clear the token from local storage
        localStorage.removeItem('token');

        // Redirect or update the UI after successful logout
        alert('Logged out successfully!');
        window.location.href = './login.html'; // Redirect to login page
    }
    catch(error){
        console.error('Error during logout:', error);
        alert('Logout failed. Please try again.');
    }

});