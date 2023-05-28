const request = require('supertest');
const data1 = require('../../../examples/test-registration-largeblock.json')
const fs = require("fs");
const readline = require("readline");

var data = [];
var url = 'https://pfn8zf2gtu.us-east-2.awsapprunner.com'


async function readfiles() {
  return new Promise((resolve, reject) => {
    const stream = fs.createReadStream("model/number_of_AS.csv");
    const rl = readline.createInterface({ input: stream });

    rl.on("line", (row) => {
      const [name, value] = row.split(",");
      const cleanedName = name.replace("#", "");

      data.push({ name: cleanedName, value });   

     });

    rl.on("close", () => {
      // console.log(data);
      console.log("closing")
      resolve(data);
    });

    rl.on("error", (err) => {
      reject(err);
    });
  });
}

    describe('POST /get-collisions', () => {
      it('check if the server is up and running', async () => {
    
        
        // Make the HTTP request
        const res = await request(url)
          .post('/get-collisions')
          .send(data1);
    
        // Check if the response code is 200 ensuring server is up and running
        expect(res.status).toEqual(200);
      });
    });

    describe('POST /get-collisions', () => {
      jest.setTimeout(30000);

      it('should get the collisions', async () => {
        // Call the readfiles function and wait for it to complete
        const data = await readfiles();
    
        // Use the data array
        for (const item of data) {
          console.log(`Name: ${item.name}, Value: ${item.value}`);
          const organ = item.name;
          const count = item.value
          data1["placement"]["target"] = `http://purl.org/ccf/latest/ccf.owl#${organ}`
          // Make the HTTP request
          const res = await request(url)
          .post('/get-collisions')
          .send(data1);
  
        // Check the response
        expect(res.body.length.toString()).toEqual(count);
    }
  });
  });
    


