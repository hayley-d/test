console.log("register.js is running!");

// register.js
document.getElementById("log").addEventListener("submit", async (e) => {
  e.preventDefault();

  // Collect form data
  const name = document.querySelector("input[name='fname']").value.trim();
  const surname = document.querySelector("input[name='lname']").value.trim();
  const phone = document.querySelector("input[name='phone']").value.trim();
  const email = document.querySelector("input[name='email']").value.trim();
  const password = document.querySelector("input[name='password']").value.trim();

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

  if (!name || !name.trim()) {
    showError(document.querySelector("input[name='fname']"), "First name is required");
    valid = false;
  }  
  
  if (!surname || !surname.trim()) {
    showError(document.querySelector("input[name='lname']"), "Last name is required");
    valid = false;
  }
  
  if (!phone || !phone.match(/^\d{10}$/)) {
    showError(document.querySelector("input[name='phone']"), "Enter a valid 10-digit phone number");
    valid = false;
  }
  
  if (!email || !email.match(/^[^\s@]+@[^\s@]+\.[^\s@]+$/)) {
    showError(document.querySelector("input[name='email']"), "Enter a valid email address");
    valid = false;
  }  

  if (!password || password.length < 8) {
    showError(document.querySelector("input[name='password']"), "Password must be at least 8 characters");
    valid = false;
  }  

  if (!valid) return;

  // Handle the response
  const response = await fetch('http://localhost:3000/register', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({
      name: name,
      surname: surname,
      phone: phone,
      email: email,
      password: password,
    }),
  });

    const result = await response.json();
    if (response.ok) {
      alert('Registration successful!');
      window.location.href = './login.html';
    } else {
      showError(email, result.error || 'Registration failed');
    }
  });

// Add some CSS for red borders
const style = document.createElement("style");
style.innerHTML = `.error { border: 2px solid red; }`;
document.head.appendChild(style);
