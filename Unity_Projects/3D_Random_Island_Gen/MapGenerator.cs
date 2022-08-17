using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;


public class MapGenerator : MonoBehaviour 
{
	//ints
	//the seed of the map is the location of the noise map that is used
	public int seed;
	//octaves changes the shape of the noise in the area
	public int octaves;
	[Range(0,1)]
	//the map is just one chunk big, use this for changing the level of detail
	const int mapChunkSize = 241;
	[Range(0,6)]
	//allows for changes in level of detail
	public int levelOfDetail;


	//floats
	//how zoomed in or zoomed out of the noise map the level is
	public float noiseScale;
	//changes the noise map features
	public float persistance;
	//changes the noise map features
	public float lacunarity;
	//affects the height of the 3D map
	public float meshHeightMultiplier;
	//the falloff map to create an area of water around the side of the map
	public float[,] foMap;

	//bools
	//used for testing purposes
	public bool autoUpdate;
	//whether the falloff map is used or not
	public bool useFOMap;

	//objects
	//the plane that the noise map and falloff map are generated on 
	public GameObject thePlane;
	//the mesh that turns the map 3D
	public GameObject theMesh;
	//the player controller
	public GameObject thePlayer;
	//the map view camera
	public GameObject theCamView;
	

	//others
	//generates the noise map, colour map, falloff map, and mesh
	public enum DrawMode {NoiseMap, ColourMap, Mesh, FOMap};
	//same as above
	public DrawMode drawMode;
	//used in generating the map
	public Vector2 offset;	
	//curve that changes the shape of the landscape from the unity inspector
	public AnimationCurve meshHeightCurve;
	public TerrainType[] regions;
	//gets the object generating script
	public RaycastingUFO theUFONow;

	//gets a reference to the game master script for keeping the score.
	public GameObject theGM;
	public GameMasterScript GMScript;



	//-----------------------------------------------------------functions---------------------------------------------------------------------------

	//on the start up
	void Awake()
	{
		//generate the falloff map
		foMap = FallOffGenerator.GenerateFOMap (mapChunkSize);
		//set the player to inactive
		thePlayer.SetActive(false);
		
	}

	//at the start of the scene
	public void Start() 
	{
		//begins the IEnumerator TheMapGen and begins the process of generating the level
		StartCoroutine (TheMapGen());

		//get the object tagged with GM
		theGM = GameObject.FindGameObjectWithTag("GM");
		GMScript = (GameMasterScript) theGM.GetComponent(typeof(GameMasterScript));
	}

	//this is where all the level generation actually happens
	public IEnumerator TheMapGen()
	{
		//set the seed to a randon number within the set range, this will change every time 
		//a different seed means a different part of the noise map is selected each time
		seed = Random.Range(0, 2147483647);
		//this sets how far in on the noise map the level is focused on, setting it to a random range between 10 and 25
		//means that the map is not too far zoomed out and looks like a series of sharp spikes, or too zoomed in and looks flat and lifeless
		noiseScale = Random.Range(10, 25);
		//sets the octaves to a random range, allowing for a varied set of shapes on the noise map within the same seed
		//works alongside persistance to change how the map looks
		octaves = Random.Range(1, 10);
		//sets the persistance to a float between 0 and 1, and works alongside octaves (the more octaves the more obvious the effect)
		//to change the overall appearance of the map
		persistance = Random.Range(0f, 1f);

		//set the use of the falloff map to off for aesthetic purposes
		useFOMap = false;

		//set the plane object to active for aesthetic purposes and set the mesh to inactive for aestehtic purposes
		//this way you get to see the map actually generate from the start
		thePlane.SetActive(true);
		theMesh.SetActive(false);
		
		//now actually begin to generate the level, first starting with the noisemap on the plane
		//do this by setting the draw mode to NoiseMap then running the GenerateMap function to show the noisemap on the plane 
		drawMode = DrawMode.NoiseMap;
		GenerateMap();
		//wait for 1 second before starting the next stage of map generation
		yield return new WaitForSeconds(1);
		//now set the draw mode to that of the colour map to generate a 2D coloured level layout that uses values between 0 and 1 in
		//order to choose the correct colour from within the regions array, then again run the GenerateMap function
		drawMode = DrawMode.ColourMap;
		GenerateMap();
		//wait for 1 second before starting the next part
		yield return new WaitForSeconds(1);
		//now generate the mesh, start by setting the mesh to active so it appears on the screen
		//then set the draw mode to mesh and run the generate map to generate a 3D level using the values from the colour map to change
		//the height of the areas in the map accordingly, and again run the GenerateMap function to make this appear
		theMesh.SetActive(true);
		drawMode = DrawMode.Mesh;
		GenerateMap();
		//wait for 1 second then disable the plane from view, then wait another second before starting the next part of the map gen process
		yield return new WaitForSeconds(1);
		thePlane.SetActive(false);
		yield return new WaitForSeconds(1);
		//set the use of the falloff map to true, this will cause the falloff map to subtract from the regular noise map setting the outer areas
		//to the lowest possible value resulting in a body of deep water surrounding the map, then run the GenerateMap function to add the changes
		//leaving a rocky island in the middle
		useFOMap = true;
		GenerateMap();
		yield return new WaitForSeconds(1);
		//add a collider to the mesh at this stage of the generation, doing it any earlier causes the collider to not correctly fit the level
		//and allows the player to fall through what should be solid objects
		MeshCollider mapMesh = theMesh.AddComponent(typeof(MeshCollider)) as MeshCollider; 
		//wait a second then call the function to have the UFO start spawning objects in the map
		yield return new WaitForSeconds(1);
		theUFONow.UFOMoving();

	}

	public void GivePlayerControl()
	{
		//now disable the map generation camera and enable the player
		theCamView.SetActive(false);
		thePlayer.SetActive(true);
		
	}


	//the generate map function is the core part of creating the level, it is used repeatedly to create the level
	public void GenerateMap() 
	{
		//set a 2d float array for the noise map, takes in a lot of information allowing all of it to be used later with less lines of code repeated
		//access the noise script and feeds this information into it too
		float[,] noiseMap = Noise.GenerateNoiseMap (mapChunkSize, mapChunkSize, seed, noiseScale, octaves, persistance, lacunarity, offset);

		//set a colour array for the colours used when generating the map on the second stage of level generation
		Color[] colourMap = new Color[mapChunkSize * mapChunkSize];

		//use a for loop to go through the level bit by bit 1 at a time
		for (int y = 0; y < mapChunkSize; y++) 
		{
			for (int x = 0; x < mapChunkSize; x++) 
			{
				//if the use of the falloff map is set to true
				if (useFOMap)
				{
					noiseMap [x, y] = Mathf.Clamp01(noiseMap [x, y] - foMap [x, y]);
				}

				//the float for setting the height of the 3D level used in generating the correct colour
				float currentHeight = noiseMap [x, y];

				//use another for loop to go through the regions (the colour scheme of the level)
				for (int i = 0; i < regions.Length; i++) 
				{
					//if the current height float is smaller to or equal to settings for the region...
					if (currentHeight <= regions [i].height) 
					{
						//...sets the colour of the area to the corresponding colour
						colourMap [y * mapChunkSize + x] = regions [i].colour;
						break;
					}
				}
			}
		}

		//this uses the mapdisplay script in order to correctly generate the appropriate part of the level generation
		//start by setting up a reference to the MapDisplay script and call it display
		MapDisplay display = FindObjectOfType<MapDisplay> ();
		
		//in hindsight this would probably have been much more effectively written if I had used a Switch statement
		
		//if the drawmode is set to noise map, then use the DrawTexture function from the map display script
		//and take in the information from the TextureFromHeightMap function from the TextureGenerator script and noisemap
		//creating a black and white noisemap on the plane object
		if (drawMode == DrawMode.NoiseMap) 
		{
			display.DrawTexture (TextureGenerator.TextureFromHeightMap (noiseMap));
		} 
		//if the drawmode is set to colourmap then use the DrawTexture function from the mapdisplay script
		//and take in information from the texturefromcolourmap function of the texturegenerator script, as well as mapchunksize
		//by mapchunk size (the size of the level by the size of the level) creating a 2D colour map on the plane
		else if (drawMode == DrawMode.ColourMap) 
		{
			display.DrawTexture (TextureGenerator.TextureFromColourMap (colourMap, mapChunkSize, mapChunkSize));
		} 
		//if the drawmode is set to mesh, then use the DrawMesh function from the map display script
		//take in the generateterrainmesh function from the meshgenerator script and feed into that the information of the noise map, the amount
		//to multiply the level by on the Z axis, the curve that changes the steepness and the level of detail (how many vertices there are on the mesh)
		//then use the information from the colourmap to set the colours to the areas in the vertices from the regions 
		else if (drawMode == DrawMode.Mesh) 
		{
			display.DrawMesh (MeshGenerator.GenerateTerrainMesh (noiseMap, meshHeightMultiplier, meshHeightCurve, levelOfDetail), TextureGenerator.TextureFromColourMap (colourMap, mapChunkSize, mapChunkSize));
		}
		//this just draws the falloff map on the plane for use on the plane in creating the outer edges of the level
		else if (drawMode == DrawMode.FOMap)
		{
			display.DrawTexture(TextureGenerator.TextureFromHeightMap(FallOffGenerator.GenerateFOMap(mapChunkSize)));
		}
	}

	void OnValidate() 
	{
		//stops the lacunarity and the octaves from going to values that will cause the level to either be so erratic and unplayable
		//and then sets it to the lowest possible value 1 and 0 respectively
		if (lacunarity < 1) {
			lacunarity = 1;
		}
		if (octaves < 0) {
			octaves = 0;
		}
	}

	void Update()
	{
	
		//if the player goes below 5 on the y axis, then...
		if (thePlayer.transform.position.y < 6)
		{
			GMScript.YouDied();
		}
		
	}

	//set the score values in here  

}

//this holds the information for the various regions of the map, water, grass, mountains etc
[System.Serializable]
public struct TerrainType {
	public string name;
	public float height;
	public Color colour;
}

