// server.js
import express from 'express';
import cors from 'cors';
import jwt from 'jsonwebtoken';
import {registerUser , loginUser , logoutUser} from './../backend/auth.js';
import dotenv from 'dotenv';
import axios from 'axios';
import bodyParser from 'body-parser';

dotenv.config();

const app = express();

app.use(cors()); //allows for cross-origin requests

app.use(express.json()); 


//for REST API
const cppApiUrl = 'http://localhost:3000/QRY'; //change to directory of C++ API


const JWT_SECRET = process.env.JWT_SECRET;

// -------------------------------------------- Register User ------------------------------------------
app.post('/register', async (req, res) => {
  const { name, surname, phone, email, password } = req.body;

  const result = await registerUser(name, surname, phone, email, password);

  if (result.error) {
    return res.status(400).json({ error: result.error });
  }

  return res.status(200).json(result);
});


// -------------------------------- Verify Token Middleware ------------------------------------
function verifyToken(req, res, next) {
  const token = req.headers['authorization'];

  if (!token) {
    return res.status(401).json({ error: 'Access denied. No token provided.' });
  }

  try {
    const decoded = jwt.verify(token, JWT_SECRET);
    req.user = decoded;
    next();
  } catch (err) {
    return res.status(400).json({ error: 'Invalid token' });
  }
}

// ------------------------------------ Protected Route Example ------------------------------------
app.get('/protected', verifyToken, (req, res) => {
  res.status(200).json({ message: 'This is a protected route', user: req.user });
});

app.get('/', (req, res) => {
  res.send('Welcome to the MPDB API!');
});

 
// Login endpoint
app.post('/login', async (req, res) => {
    const { email, password } = req.body;
    
    if(!email || !password){
        return res.status(400).json({error: 'Email and password are required'});
    }

    const result = await loginUser(email, password);

    if(result.error){
        return res.status(401).json({error: result.error});
    }

    //login successful
    res.status(200).json(result);

});

// Logout endpoint
app.post('/logout', async (req, res) => {
    const {token} = req.body; //get the token from the request body

    if(!token){
        return res.status(400).json({error: 'Token is required'});
    }

    const result = await logoutUser(token);

    if (result.error) {
      return res.status(500).json({ error: result.error });
  }
  return res.status(200).json(result);
  

    
});

app.post('/QRY', async (req, res) => {

  const receivedData = req.body;

  //Debugging
  console.log('Received Data:', receivedData);

  //Validate request body
  if (!receivedData.query || receivedData.query.trim() === '') {
    return res.status(400).json({
      success: false,
      message: 'Query cannot be empty',
    });
  }

  try {
    // Mock userID and server_secret (ignore them for now)
    const mockUserID = 'Blackbeard';
    const mockServerSecret = 'as)#)(AFJS82)24121SKLFIU9322386656fj209KKJJDAAKJKII';
    
    // Process the query (replace this with your actual logic)
    const processedData = {
      status: 'success',
      data: {
        query: receivedData.query,
        result: `Processed query: ${receivedData.query}`,
        userID: mockUserID, // Mocked userID
        server_secret: mockServerSecret, // Mocked server_secret
      },
    };
    
    // Debugging: Log the processed data
    console.log('Processed Data:', processedData);
    
    // Send the processed data back to the client
    res.status(200).json(processedData);
    
  } catch (error) {
    // Debugging: Log the full error
    console.error('Error during query processing:', error);
    
    // Send an error response
    res.status(500).json({
      success: false,
      message: 'Error during query processing',
      error: error.message,
    });
  }
});




// ------------------------------------------------
//               CLI changes
// ------------------------------------------------

app.use(bodyParser.json());
    
app.post('/QRY', (req, res) => {
  const { userID, server_secret, query } = req.body;

  if (!userID || !server_secret || !query) {
    return res.json({
      success: false,
      message: "Missing userID/server_secret/query",
      data: []
    });
  }

  if (query.startsWith('GET')) {
    return res.json({
      success: true,
      message: '',
      data: [
        {
            "id" : 1,
            "data" :    {
                            "name" : "Jack Sparrow",
                            "age" : 27,
                            "hobbies" : ["Pillaging", "Drinking", "Cheating", "Tortuga"],
                            "ship" : {
                                            "name" : "The Black Pearl",
                                            "type" : "Frigate",
                                            "cannons" : 23
                                        }
                        }
        },

        {
            "id" : 17,
            "data" :    {
                            "name" : "Hector Barbossa",
                            "age" : 43,
                            "hobbies" : ["Sleeping", "Getting Cursed", "Losing a leg"],
                            "ship" : {
                                            "name" : "The Black Pearl",
                                            "type" : "Frigate",
                                            "cannons" : 23
                                        }
                        }   
                        
        }
    ]
    });
  } 
  else if (query.startsWith('SET')) {
    return res.json({
      success: true,
      message: '',
      data: [{ id: 999, data: { note: 'Some SET data' } }]
    });
  }
  else if (query.startsWith('DELETE')) {
    return res.json({
      success: true,
      message: '',
      data: [{ id: 1, data: { note: 'Some DELETE data' } }]
    });
  }
  else if (query.startsWith('UPDATE')) {
    return res.json({
      success: true,
      message: '',
      data: [{ id: 77, data: { note: 'Some UPDATE data' } }]
    });
  }
  else {
    return res.json({
      success: false,
      message: 'Unrecognized query type',
      data: []
    });
  }
});

// Start the server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  //console.log(process.env.SUPABASE_URL);  
  console.log(`Server running on http://localhost:${PORT}`);
});
