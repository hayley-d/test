import express from 'express';
import bodyParser from 'body-parser';


export function createFakeServer() {
  const app = express();
  app.use(bodyParser.json());


  app.post('/QRY', (req, res) => {
    const { userID, server_secret, query } = req.body;

    // missing body parts... 0_0
    if (!userID || !server_secret || !query) {
      return res.json({
        success: false,
        message: "Missing userID/server_secret/query",
        data: []
      });
    }

    // response for valid GET
    if (query.startsWith('GET')) {
      return res.json({
        success: true,
        message: '',
        data: [{ id: 1, data: { note: 'Fake GET data' } }]
      });
    } 
    // response for valid SET
    else if (query.startsWith('SET')) {
      return res.json({
        success: true,
        message: '',
        data: [{ id: 999, data: { note: 'Fake SET data' } }]
      });
    }
    // response for valid delete 
    else if (query.startsWith('DELETE')) {
      return res.json({
        success: true,
        message: '',
        data: [{ id: 1, data: { note: 'Fake DELETE data' } }]
        // data: []
      });
    }
    // response for valid update
    else if (query.startsWith('UPDATE')) {
      return res.json({
        success: true,
        message: '',
        data: [{ id: 1, data: { note: 'Fake UPDATE data' } }]
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

  return app;
}
