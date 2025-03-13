import { expect } from 'chai';
import http from 'node:http';
import { createFakeServer } from './fakeServer.js';
import { spawn } from 'node:child_process';
import path from 'node:path';

import { fileURLToPath } from 'url';
import { dirname } from 'path';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const cliPath = path.join(__dirname, '..', 'mpdb.js');

let server;

describe('CLI Tests', function() {
  // to accept requests at http://localhost:8080/QRY/
  before(function(done) {
    const app = createFakeServer();
    server = http.createServer(app);
    server.listen(8080, () => {
      console.log('[TEST] Fake server running on port 8080');
      done();
    });
  });

  // shutdown after tests
  after(function(done) {
    server.close(() => {
      console.log('[TEST] server stopped');
      done();
    });
  });

  // -----------------------------------------------------
  //                 GET QUERY TEST
  // -----------------------------------------------------
  it('should handle GET query', function(done) {

    // run node mpdb.js in a child process
    const cli = spawn('node', [cliPath]);
    let sentAuth = false;
    let sentPassword = false;
    let sentGet = false;
    let handledServer = false;

    
    // store cli output
    let outputBuffer = ''; 
    cli.stdout.on('data', (data) => {
      const chunk = data.toString();
      // console.log('[CLI OUTPUT]', JSON.stringify(chunk)); 

      // outputBuffer += data.toString();
      outputBuffer += chunk;

      // login
      if (!sentAuth && outputBuffer.includes('mpdb>')) {
        cli.stdin.write('AUTH Alex\n');
        sentAuth = true;
      }

      // enter password
      if (!sentPassword && outputBuffer.includes('Enter password: ')) {
        cli.stdin.write('AlexPassword\n');
        sentPassword = true;
      }

      // send GET query
      if (!sentGet && outputBuffer.includes('[INFO] Authentication successful.')) {
        cli.stdin.write(`GET PIRATES\n`);
        sentGet = true;
      }

      // check if query is successfully sent to fake server and receives correct response 
      if (!handledServer && outputBuffer.includes('[SERVER] success:')) {
        expect(outputBuffer).to.include('Fake GET data');
        cli.stdin.write('EXIT\n');
        handledServer = true;
      }

    });

    cli.stderr.on('data', (errData) => {
      console.error('[CLI ERROR]', errData.toString());
    });

    cli.on('close', (code) => {
      expect(code).to.equal(0);
      done();
    });
  });


  // ----------------------------------------------------------
  //                       SET QUERY TESTING
  // ----------------------------------------------------------
  it('should handle SET query', function(done) {
    const cli = spawn('node', [cliPath]);
    let outputBuffer = '';
    let sentAuth = false;
    let sentPassword = false;
    let sentSet = false;
    let handledServer = false;

    cli.stdout.on('data', (data) => {
      const chunk = data.toString();
      outputBuffer += chunk;

      if (!sentAuth && outputBuffer.includes('mpdb>')) {
        cli.stdin.write('AUTH Alex\n');
        sentAuth = true;
      }

      if (!sentPassword && outputBuffer.includes('Enter password: ')) {
        cli.stdin.write('AlexPassword\n');
        sentPassword = true;
      }

      if (!sentSet && outputBuffer.includes('[INFO] Authentication successful.')) {
        cli.stdin.write(`SET PIRATES '{ "name": "Blackbeard" }'\n`);
        sentSet = true;
      }

      if (!handledServer && outputBuffer.includes('[SERVER] success:')) {
        expect(outputBuffer).to.include('Fake SET data');
        cli.stdin.write('EXIT\n');
        handledServer = true;
      }
    });

    cli.stderr.on('data', (errData) => {
      console.error('[CLI ERROR]', errData.toString());
    });

    cli.on('close', (code) => {
      expect(code).to.equal(0);
      done();
    });
  });


  // --------------------------------------------------------
  //               DELETE QUERY TESTING
  // --------------------------------------------------------
  it('should handle DELETE query', function(done) {
    const cli = spawn('node', [cliPath]);
    let outputBuffer = '';
    let sentAuth = false;
    let sentPassword = false;
    let sentDelete = false;
    let handledServer = false;

    cli.stdout.on('data', (data) => {
      const chunk = data.toString();
      outputBuffer += chunk;

      if (!sentAuth && outputBuffer.includes('mpdb>')) {
        cli.stdin.write('AUTH Alex\n');
        sentAuth = true;
      }

      if (!sentPassword && outputBuffer.includes('Enter password: ')) {
        cli.stdin.write('AlexPassword\n');
        sentPassword = true;
      }

      if (!sentDelete && outputBuffer.includes('[INFO] Authentication successful.')) {
        cli.stdin.write(`DELETE ship.nickname GET PIRATES ship.nickname == 'Small Boat'\n`);
        sentDelete = true;
      }

      if (!handledServer && outputBuffer.includes('[SERVER] success:')) {
        expect(outputBuffer).to.include('Fake DELETE data');
        cli.stdin.write('EXIT\n');
        handledServer = true;
      }
    });

    cli.stderr.on('data', (errData) => {
      console.error('[CLI ERROR]', errData.toString());
    });

    cli.on('close', (code) => {
      expect(code).to.equal(0);
      done();
    });
  });



  // --------------------------------------------------
  //            UPDATE QUERY TESTING
  // --------------------------------------------------
  it('should handle UPDATE query', function(done) {
    const cli = spawn('node', [cliPath]);
    let outputBuffer = '';
    let sentAuth = false;
    let sentPassword = false;
    let sentUpdate = false;
    let handledServer = false;

    cli.stdout.on('data', (data) => {
      const chunk = data.toString();
      outputBuffer += chunk;

      if (!sentAuth && outputBuffer.includes('mpdb>')) {
        cli.stdin.write('AUTH Alex\n');
        sentAuth = true;
      }

      if (!sentPassword && outputBuffer.includes('Enter password: ')) {
        cli.stdin.write('AlexPassword\n');
        sentPassword = true;
      }

      if (!sentUpdate && outputBuffer.includes('[INFO] Authentication successful.')) {
        cli.stdin.write(`UPDATE PIRATES days_since_last_bath '++1' GET PIRATES ship.name == 'The Flying Dutchmen'\n`);
        sentUpdate = true;
      }

      if (!handledServer && outputBuffer.includes('[SERVER] success:')) {
        expect(outputBuffer).to.include('Fake UPDATE data');
        cli.stdin.write('EXIT\n');
        handledServer = true;
      }
    });

    cli.stderr.on('data', (err) => {
      console.error(err.toString());
    });

    cli.on('close', (code) => {
      expect(code).to.equal(0);
      done();
    });
  });


  // ----------------------------------------
  //          INVALID GET SYNTAX
  // ----------------------------------------
  it('should handle invalid GET syntax', function(done) {
    const cli = spawn('node', [cliPath]);
    let outputBuffer = '';
    let sentAuth = false;
    let sentPassword = false;
    let sentBadGet = false;
    let capturedError = false;

    cli.stdout.on('data', (data) => {
      const chunk = data.toString();
      outputBuffer += chunk;
      if (!sentAuth && outputBuffer.includes('mpdb>')) {
        cli.stdin.write('AUTH Alex\n');
        sentAuth = true;
      }
      if (!sentPassword && outputBuffer.includes('Enter password: ')) {
        cli.stdin.write('AlexPassword\n');
        sentPassword = true;
      }
      if (!sentBadGet && outputBuffer.includes('[INFO] Authentication successful.')) {
        cli.stdin.write(`GET PIRATES == "Big Ship"\n`);
        sentBadGet = true;
      }
      if (!capturedError && outputBuffer.includes('[ERROR] GET syntax: found operator == but no <key> before it.')) {
        capturedError = true;
        cli.stdin.write('EXIT\n');
      }
    });

    cli.stderr.on('data', (err) => {
      console.error(err.toString());
    });

    cli.on('close', (code) => {
      expect(code).to.equal(0);
      expect(capturedError).to.equal(true);
      done();
    });
  });


});
