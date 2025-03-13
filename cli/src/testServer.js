import express from 'express';
import bodyParser from 'body-parser';

const app = express();
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

app.listen(3000, () => {
  console.log('[Test Server] running on port 3000');
});