const request = require('supertest');
const data1 = require('../../../examples/test-registration-largeblock.json')
const fs = require("fs");
const readline = require("readline");

var data = [];
var url = 'https://pfn8zf2gtu.us-east-2.awsapprunner.com'

function getRandomIndices(numElements, arrayLength) {
  const indices = new Set();
  while (indices.size < numElements) {
    const randomIndex = Math.floor(Math.random() * arrayLength);
    indices.add(randomIndex);
  }
  return Array.from(indices);
}


async function readVertexFile(filename) {
  return new Promise((resolve, reject) => {
    const stream = fs.createReadStream(filename);
    const rl = readline.createInterface({ input: stream });

    const vertices = [];
    let isHeaderPassed = false; // Flag to skip the first two lines

    rl.on("line", (line) => {
      if (!isHeaderPassed) {
        isHeaderPassed = true; // Skip the first line ("OFF") and the second line (counts)
        return;
      }

      const parts = line.trim().split(/\s+/);
      if (parts.length === 3) { // Ensure there are three parts for x, y, z
        vertices.push({
          x: parseFloat(parts[0]*1000),
          y: parseFloat(parts[1]*1000),
          z: parseFloat(-1*parts[2]*1000)
        });
      }
    });

    rl.on("close", () => resolve(vertices));
    rl.on("error", (err) => reject(err));
  });
}

function calculateCoordinateDifferences(vertices, origin) {
  return vertices.map(vertex => ({
    diff_x: vertex.x - origin.x_origin,
    diff_y: vertex.y - origin.y_origin,
    diff_z: vertex.z - origin.z_origin
  }));
}

async function readfOriginDataFile(filename) {
  return new Promise((resolve, reject) => {
    const stream = fs.createReadStream(filename);
    const rl = readline.createInterface({ input: stream });

    const data = [];
    rl.on("line", (row) => {
      if (!row.startsWith("#")) return; // Skip lines that don't start with '#'
      const [name, x_origin, y_origin, z_origin] = row.split(",").map(part => part.trim());
      data.push({
        name: name.substring(1), // Remove '#' from the name
        x_origin: parseFloat(x_origin*1000), 
        y_origin: parseFloat(y_origin*1000), 
        z_origin: parseFloat(z_origin*1000)
      });
    });

    rl.on("close", () => resolve(data));
    rl.on("error", (err) => reject(err));
  });
}
async function readfiles(filename) {
  return new Promise((resolve, reject) => {
    const stream = fs.createReadStream(filename);
    const rl = readline.createInterface({ input: stream });

    const data = []; // Ensure data is initialized here

    rl.on("line", (row) => {
      const [name, value] = row.split(",");
      const cleanedName = name.replace("#", "");

      data.push({ name: cleanedName, value });
    });

    rl.on("close", () => {
      console.log("closing");
      resolve(data);
    });

    rl.on("error", (err) => {
      reject(err);
    });
  });
}
function getOrganCoordinates(data, organName) {
  // Convert the search organ name to lowercase for case-insensitive comparison
  const searchName = organName.toLowerCase();

  // Find the organ data with a case-insensitive match
  const organData = data.find(item => item.name.toLowerCase() === searchName);
  
  if (!organData) {
    throw new Error(`Organ ${organName} not found`);
  }

  // Return the coordinates
  return {
    x_origin: organData.x_origin,
    y_origin: organData.y_origin,
    z_origin: organData.z_origin
  };
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
        const data = await readfiles("model/number_of_AS.csv");
    
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



    describe('POST /get-collisions', () => {
      jest.setTimeout(30000);

      it('For a small tissue block', async () => {
        // Call the readfiles function and wait for it to complete
        const organOriginsData = await readfOriginDataFile("model/organ_origins_meter_v1.4.csv");
        const verticesData = await readVertexFile("model/plain_manifold_filling_hole_v1.4/VH_F_Kidney_L/VH_F_renal_pyramid_L_a.off")
        const originCoords = getOrganCoordinates(organOriginsData,"VHMLeftKidney")
        const coordinateDifferences = calculateCoordinateDifferences(verticesData, originCoords);
        const randomIndices = getRandomIndices(5, coordinateDifferences.length);

        console.log(coordinateDifferences.length)
        // console.log(verticesData)
        // Use the data array
        const count = 0
        for (const diff of randomIndices) {
            // console.log(`Name: ${item.name}, Value: ${item.value}`);
            data1["placement"]["x_translation"] = diff.diff_x
            data1["placement"]["x_translation"] = diff.diff_y
            data1["placement"]["x_translation"] = diff.diff_z
            data1["x_dimension"] = 1
            data1["y_dimension"] = 1
            data1["z_dimension"] = 1
            // Make the HTTP request
            const res = await request(url)
            .post('/get-collisions')
            .send(data1);
    
          // Check the response
          expect(res.body.length).toBeLessThanOrEqual(1);
        
      }
  });
  });