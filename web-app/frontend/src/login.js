document.getElementById('log').addEventListener('submit', async (event) => {
    event.preventDefault(); // Prevent the form from submitting the traditional way

    const email = document.getElementById('email').value;
    const password = document.getElementById('password').value;

    document.querySelectorAll(".error-message").forEach(el => el.remove());
    document.querySelectorAll(".box").forEach(el => el.classList.remove("error"));

    let valid = true;

    function showError(input, message) {
        const error = document.createElement("p");
        error.classList.add("error-message");
        error.style.color = "red";
        error.style.fontSize = "12px";
        error.innerText = message;
        input.classList.add("error");
        input.parentNode.insertBefore(error, input.nextSibling);
    }

    if (!email || !email.match(/^[^\s@]+@[^\s@]+\.[^\s@]+$/)) {
        showError(document.getElementById('email'), "Enter a valid email address");
        valid = false;
    }

    if (!password || password.length < 8) {
        showError(document.getElementById('password'), "Password must be at least 8 characters");
        valid = false;
    }

    if (!valid) return;

    try {
        const response = await fetch('http://localhost:3000/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ email, password }),
        });

        if (!response.ok) {
            // Handle HTTP errors (e.g., 401 Unauthorized, 400 Bad Request)
            const errorData = await response.json();
            throw new Error(errorData.error || 'Login failed');
        }

        const data = await response.json();
        console.log('Login successful:', data);

        // Save the token in local storage
        localStorage.setItem('token', data.token);

        // Redirect or update the UI after successful login
        alert('Login successful!');

        //will have to change this to the dashboard page (used home.html for now)
        window.location.href = './home.html';// Redirect to a dashboard or home page 


    } catch (error) {
        console.error('Error during login:', error);

        document.getElementById('errorMessage').textContent = error.message; // Display error message
    }
});