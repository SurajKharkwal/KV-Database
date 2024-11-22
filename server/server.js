import express, { json } from 'express';
import { exec } from 'child_process';
import path from 'path';

// Get the current directory of the file
const __dirname = path.dirname(new URL(import.meta.url).pathname);

const app = express();
app.use(json());

// Serve React build
app.use(express.static(path.join(__dirname, '../client/dist/')));
// Run command function
const runCommand = (command, callback) => {
  const process = exec(command, (error, stdout, stderr) => {
    if (error) {
      console.error(`exec error: ${error}`);
      return;
    }
    callback(stdout.trim());
  });
};

// Routes for key-value operations
app.post('/setKv', (req, res) => {
  const { key, value } = req.body;
  runCommand(
    `./kvstore insertKv ${key} ${value}`,
    (result) => res.send(result)
  );
});

app.post('/deleteKv', (req, res) => {
  const { key } = req.body;
  runCommand(
    `./kvstore deleteKv ${key}`,
    (result) => {
      console.log(result);
      res.send(result)
    }
  );
});

app.post('/updateKv', (req, res) => {
  const { key, value } = req.body;
  console.log(key, value)
  runCommand(
    `./kvstore updateKv ${key} ${value}`,
    (result) => {
      console.log("Result: ", result);
      res.send(result)
    }
  );
});

app.post('/searchKv', (req, res) => {
  const { key } = req.body;
  runCommand(
    `./kvstore searchKv ${key}`,
    (result) => res.send(result)
  );
});

app.get('/api/getAll', (req, res) => {
  runCommand(
    './kvstore getAll',
    (result) => res.send(result)
  );
});

app.get("/api/hello", (req, res) => {
  console.log(req.method)
  res.json({ message: "Hello, TypeScript!" });
});


app.get('*', (req, res) => {
  console.log(req.method)
  res.sendFile(path.join(__dirname, '../client/dist/', 'index.html'));
});



// Start the server
app.listen(3000, () => {
  console.log('Server running on http://localhost:3000');
});

