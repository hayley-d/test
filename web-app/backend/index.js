import express from 'express';
import axios from 'axios';
import dotenv from 'dotenv';

dotenv.config(); //load environment variables

const app = express();
const port = 3001; //make different to port used in C++ server
const cppApiUrl = 'http://localhost:3000/QRY'; //change to directory of C++ API

app.use(express.json()); //allows for parsing json

async function sendQueryToDaemon(userID, server_secret, query){

    try{

        const response = await axios.post(cppApiUrl, {
            userID, 
            server_secret: process.env.server_secret,
            query
        });

        return response.data;

    } catch(error){

        console.error("Request error: " + error);

        return {success: false, message: "Error: request to C++ Daemon failed", data: []};

    }

}

app.post('/query', async (req, res) => {

    const {userID, query} = req.body;

    if(!userID || !query){

        return res.status(400).json({success: false, message: "UsersID or Query missing from request"});

    }

    const response = await sendQueryToDaemon(userID, query);
    res.json(response);

})

app.listen(port, () => {

    console.log(`Connected from port ${port}.`);

})