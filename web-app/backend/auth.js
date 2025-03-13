import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';
import dotenv from 'dotenv';
dotenv.config();
import { supabase } from './db.js';

const JWT_SECRET = process.env.JWT_SECRET;


const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

function validateEmail(email) {
  return emailRegex.test(email);
}

function validatePassword(password) {
  const minLength = 8;
  const hasUpperCase = /[A-Z]/.test(password);
  const hasLowerCase = /[a-z]/.test(password);
  const hasNumbers = /[0-9]/.test(password);
  const hasNonAlphas = /[!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?]/.test(password);

  if(password.length < minLength){
    return 'Password must be at least 8 characters long.';
  }
  
  if(!hasUpperCase){
    return 'Password must contain at least one uppercase letter.';
  }
  if (!hasLowerCase) {
    return 'Password must contain at least one lowercase letter.';
  }
  if (!hasNumbers) {
    return 'Password must contain at least one number.';
  }
  if (!hasNonAlphas) {
    return 'Password must contain at least one special character.';
  }

  return null; // Password is valid
}

// -------------------------------------------- Register User ------------------------------------------
async function registerUser(name, surname, phone, email, password) {

  //validate email
  if (!validateEmail(email)) {
    console.error('Invalid email');
    return { error: 'Invalid email' };
  }

  //validate password
  const passwordError = validatePassword(password);
  if(passwordError){
    console.error('Invalid password:', passwordError);
    return {error: passwordError};
  }
  
  const hashedPassword = await bcrypt.hash(password, 10);

  // Insert without specifying `id` so it can be generated automatically
  const { data, error } = await supabase
    .from('users')
    .insert([{ 
      name, 
      surname, 
      phone,
      email, 
      password: hashedPassword }])
    .select(); // Use .select() to return the inserted record


  if (error) {
    if(error.code === '23505'){ //error code for unique constraint violation
      console.error('Email already exists');
      return { error: 'Email already exists' };
    }

    else if(error.code === '23502'){  //error code for not null constraint violation
      console.error('Missing required fields');
      return {error: 'Missing required fields'};
    }

    else{
      console.error('Error registering user:', error.message);
      return {error: ' Registration failed'};
    }
  }

  //exclude password from the data
 if (data && data.length > 0) {
    const user = data[0]; // Get the first (and only) inserted record
    const { password: _, ...userData } = user; // Exclude the password field
    return userData;
  }

  console.error('No data returned after registering user');
  return { error: 'Registration failed' };
}

// ------------------------------------------- Login User -------------------------------------------
// async function loginUser(email, password) {
//   // Fetch the user with the given email
//   const { data: user, error } = await supabase
//     .from('users')
//     .select('*')
//     .eq('email', email)
//     .single();

//   console.log('Fetched user:', user); 

//   if (error || !user) {
//     console.error('Error fetching user:', error);
//     return null;
//   }

//   // Compare the provided password with the stored hash
//   const passwordMatch = await bcrypt.compare(password, user.password);

//   if (!passwordMatch) {
//     console.error('Invalid password');
//     return null;
//   }

//   // Generate JWT token
//   const token = jwt.sign({ id: user.id, email: user.email }, JWT_SECRET, {
//     expiresIn: '30m', // Token expires in 30 minutes (changed for practical use)
//   });

//   // Return token + user data (excluding password)
//   const { password: _, ...userData } = user;
//   console.log('User data:', userData); // Log userData to check if it's correct

//   return { token, user: userData };
// }

async function loginUser(email, password) {
  // Fetch the user with the given email
  const { data, error } = await supabase
    .from('users')
    .select('*')
    .eq('email', email)
    .single();

  // Check if the user is fetched correctly
  const user = Array.isArray(data) ? data[0] : data;

  if (error || !user) {
    console.error('Error fetching user:', error);
    return null;
  }

  //console.log('Fetched user:', user);

  // Compare the provided password with the stored hash
  const passwordMatch = await bcrypt.compare(password, user.password);

  if (!passwordMatch) {
    console.error('Invalid password');
    return null;
  }

  // Generate JWT token
  const token = jwt.sign({ id: user.id, email: user.email }, JWT_SECRET, {
    expiresIn: '30m', // Token expires in 30 minutes
  });

  // Return token + user data (excluding password)
  const { password: _, ...userData } = user;
  console.log('User data:', userData);

  return { token, user: userData };
}

// ------------------------------------------- Update User Details -------------------------------------------

async function updateUserDetails(userId, updates){

  //validate userId
  if(!userId || typeof userId !== 'number'){
    console.error('Invalid userId', userId);
    return null;
  }

  //ensure that only allowed fields are updated
  const allowedUpdates = ['name', 'surname'];
  const validUpdates = Object.keys(updates).filter(key => allowedUpdates.includes(key));

  if(validUpdates.length === 0){
    console.error('Invalid updates provided');
    return null;
  }

  //Create an object with only the allowed fields
  const filteredUpdates = {};
  validUpdates.forEach(key => {
    filteredUpdates[key] = updates[key];
  });

  // Update the user record in the database
  const { data, error } = await supabase
    .from('users')
    .update(filteredUpdates)
    .eq('id', userId)
    .select();  // Use .select() to return the updated record

  if (error) {
    console.error('Error updating user:', error.message);
    return null;
  }


  //Return the updated user data (excluding the password)
  if (data && data.length > 0) {
    const user = data[0]; // Get the first (and only) updated record
    const { password: _, ...userData } = user; // Exclude the password field
    return userData;
  }

  console.error('No data returned after updating user');
  return null;

}

// -------------------------------- Verify Token Middleware ------------------------------------
function verifyToken(token) {
  try {
    return jwt.verify(token, JWT_SECRET);
  } catch (err) {
    return null;
  }
}

// module.exports = {verifyToken};

// ------------------------------------ LogoutUser ----------------------------------------------
async function logoutUser(token) {
  // going to store invalidated tokens in DB to prevent reuse ------ NB!!!!!
  // Add refresh tokens too.
  const { data, error } = await supabase
    .from('invalidated_tokens')  // Table to store invalidated tokens
    .insert([{ token }]); // Insert the token into the table
  
  if (error) {
    console.error('Error invalidating token:', error.message);
    return { message: 'Logout failed.' };
  }

  return { message: 'Logged out successfully' };
}

async function isTokenValid(token) {
  const { data, error } = await supabase
    .from('invalidated_tokens')
    .select('*')
    .eq('token', token)
    .single();

  if (error) {
    console.error('Error checking token validity:', error.message);
    return false; // If there's an error, assume token is invalid
  }

  // If data exists, it means the token is invalid
  return !data;
}

// ============================== PASSWORD RESET POLICY ========================================
async function changePassword(userId, currentPassword, newPassword) {
  const { data: user, error } = await supabase
    .from('users')
    .select('password')
    .eq('id', userId)
    .single();

  if (error || !user) {
    console.error('User not found');
    return null;
  }

  const passwordMatch = await bcrypt.compare(currentPassword, user.password);
  if (!passwordMatch) {
    console.error('Incorrect current password');
    return null;
  }

  const passwordError = validatePassword(newPassword);
  if (passwordError) {
    console.error(passwordError);
    return null;
  }

  const hashedPassword = await bcrypt.hash(newPassword, 10);
  await supabase.from('users').update({ password: hashedPassword }).eq('id', userId);
  return { message: 'Password updated successfully' };
}

async function resetPassword(email, newPassword) {
  const { data: user, error } = await supabase
    .from('users')
    .select('id')
    .eq('email', email)
    .single();

  if (error || !user) {
    console.error('Email not found');
    return null;
  }

  const passwordError = validatePassword(newPassword);
  if (passwordError) {
    console.error(passwordError);
    return null;
  }

  const hashedPassword = await bcrypt.hash(newPassword, 10);
  await supabase.from('users').update({ password: hashedPassword }).eq('id', user.id);
  return { message: 'Password reset successfully' };
}

export { registerUser, loginUser, logoutUser, verifyToken, isTokenValid, updateUserDetails, changePassword, resetPassword, validatePassword, validateEmail };