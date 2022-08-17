using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RaycastingUFO : MonoBehaviour 
{
	//ints
	//the location of the UFO on the x axis
	public int xUFO;
	//the location of the UFO on the y axis
	public int yUFO = 1000;
	//the location of the UFO on the z axis
	public int zUFO;
	//the maximum number of objects placed on the map
	private int maxUFO;
	//the number of objects placed by the ufo
	public int placedUFO;
	//int for the number of collectibles to be placed
	public int toCollect;
	//int for the number of collectibles that has been placed
	public int collectThis;

	//floats
	//the distance the raycast has travelled
	public float distFromUFO;

	//gameobjects
	//the UFO that fires the ray down and spawns an object at it's location 
	public GameObject theUFO;
	//the beach objects array
	public GameObject[] beachObj;
	//the grassland objects array
	public GameObject[] grassObj;
	//the cliffside objects array
	public GameObject[] cliffObj;
	//the clifftop objects array
	public GameObject[] cliffTopObj;
	//the mountain objects array
	public GameObject[] mountainObj;
	//the snow objects array
	public GameObject[] snowObj;
	//the collectible in the game
	public GameObject shipPart;

	//if the raycast hits anything
	RaycastHit rayHasHit;

	//reference to the map generator script
	public MapGenerator mapGen;

	//gets a reference to the game master script for keeping the score.
	public GameObject theGM;
	public GameMasterScript GMScript;




	void Start()
	{
		//set the needed information at the start of the scene
		//since no objects have yet been placed by the UFO
		placedUFO = 0;
		//now set the max number of objects that can be placed by the ufo
		//have this randomly set for extra variation 
		maxUFO = Random.Range(50, 150);
		//set the start point for the UFO, set it to the very centre of the map
		theUFO.transform.position = new Vector3 (0, 500, 0);
		//set the maximum number of collectibles that can be placed on the map
		toCollect = Random.Range(10, 15);
		//and set the current number of collectibles placed to 0 as none have been placed at this point
		collectThis = 0;

		//get the object tagged with GM
		theGM = GameObject.FindGameObjectWithTag("GM");
		GMScript = (GameMasterScript) theGM.GetComponent(typeof(GameMasterScript));

		GMScript.GetUFO();
	

		//for testing, disable when actually running.
	//	UFOMoving();
	}

	//Decides what to do with the UFO
	public void UFOMoving()
	{
		//if the number of placed objects is less than the maximum number of objects to be placed
		//then start the coroutine so that the UFO will move to a random location on the map and fire a raycast
		if (placedUFO < maxUFO)
		{
			
			StartCoroutine (UFOWillMove());
		}
		//but if the max number of placed objects is equal to or higher than the max number of placed objects
		//and the number of collectibles is smaller than the max number of collectibles to be placed
		//then call the start the coroutine to place a collectible at a random location on the map
		else if (placedUFO >= maxUFO && collectThis < toCollect)
		{
			StartCoroutine (PlaceCollectibles());
		}
		//however if the number of placed objects is equal to, or somehow higher than the max number of placed objects
		//and the number of collectibles is equal to or higher than the max number of placed objects
		//then disable the UFO object. 
		//I used another function for this rather than disabling the bject here so that I could pause and restart a coroutine
		//in the main map generator script to stop the player spawning in as objects spawn in
		//then give information to the GameMaster and the score script
		else if (placedUFO >= maxUFO && collectThis >= toCollect)
		{
			mapGen.GivePlayerControl();
			GMScript.InitialCollect();
			DisableTheUFO();
		}

		
	}

	//the Enum that allows the UFO to move around the map
	public IEnumerator UFOWillMove()
	{
		//sets the value that the ufo will teleport to on the x axis during the transform position
		xUFO = Random.Range(-750, 751);
		//sets the value that the ufo will teleport to on the z axis during the transform position
		zUFO = Random.Range(-750, 751);
		//moves the ufo object to the position on the map axis (x, y, z)
		//in this case the x and z values will be random within a set range of -750 and 751
		//and the y axis will remain at 1000, since this was the height used for testing 
		theUFO.transform.position = new Vector3(xUFO, yUFO, zUFO);
		//start the function that fires the raycast
		FiringTheRay();
		//call the function what to spawn to choose what object gets spawned in the location
		//this will select  the correct object for the location the object will be spawned
		WhatToSPawn();
		//wait for a period of time before calling the UFOMoving function to repeat the process
		//a for loop would probably have worked but I couldn't figure this out
		yield return new WaitForSeconds (0.01f);
		UFOMoving();
		
	}

	public IEnumerator PlaceCollectibles()
	{
		//sets the value that the ufo will teleport to on the x axis during the transform position
		xUFO = Random.Range(-750, 751);
		//sets the value that the ufo will teleport to on the z axis during the transform position
		zUFO = Random.Range(-750, 751);
		//moves the ufo object to the position on the map axis (x, y, z)
		//in this case the x and z values will be random within a set range of -750 and 751
		//and the y axis will remain at 1000, since this was the height used for testing 
		theUFO.transform.position = new Vector3(xUFO, yUFO, zUFO);
		//start the function that fires the raycast
		FiringTheRay();
		//the function that checks if this is a place that can spawn a collectible
		CanPlaceShipPart();
		//wait for a period of time before calling the UFOMoving function to repeat the process
		yield return new WaitForSeconds (0.01f);
		UFOMoving();
	}

	//used for testing to get the values needed for the terrain distance
	void Update()
	{
	//	FiringTheRay();
	}

	//the raycast being fired
	public void FiringTheRay()
	{
		//if the raycast fires down from the position that the ufo is currently at and hits an object
		//then it will return the rayhashit and then...
		if (Physics.Raycast (transform.position, transform.TransformDirection (Vector3.down), out rayHasHit))
		{
			//...the distance from the start point and point where it is will be the distfromufo
			distFromUFO = rayHasHit.distance;
			Debug.Log ("Distance is " + distFromUFO);
		}
	}

	//sets what object to spawn depending on the height of the map by using the distance between the ufo
	//and the mesh that the raycast has collided with
	void WhatToSPawn()
	{
		//if the distance is greater than 997, then do nothing, this is supposed to be deep water
		if (distFromUFO > 997f)
		{
			
		}
		//but if the distance is between 997 and 995, then call the function spawn an object from the beachobject array
		//as the height works out as beach area on the map
		else if (distFromUFO <= 997f && distFromUFO > 995f)
		{
			GenerateBeachObj();
		}
		//if the distance is between 995 and 980, then call the function to spawn an object from the grassobject array 
		else if (distFromUFO <= 995f && distFromUFO > 980f)
		{
			GenerateGrassObj();
		}
		//if the distance is between 980  and 910, then call the function to spawn an object from the cliffobject array
		else if (distFromUFO <= 980f && distFromUFO > 910f)
		{
			GenerateCliffObj();
		}
		//if the distance is between 910 and 900, then call the function to spawn an object from the clifftopobject array
		else if (distFromUFO <= 910f && distFromUFO > 900f)
		{
			GenerateCliffTopObj();
		}
		//if the distance is between 900 and 810, then call the function to spawn an object from the mountainobject array
		else if (distFromUFO <= 900f && distFromUFO > 810f)
		{
			GenerateMountainObj();
		}
		//finally if the distance is equal to or below 810 then spawn an object from the snowobject array
		else if (distFromUFO <= 810f)
		{
			GenerateSnowObj();
		}
	}

	//if the location fits into either of these groups then the function will call the function to generate one of the collectible
	//ship parts in the area, however if the ufo is above a location that isn't valid a valid location nothing will happen
	void CanPlaceShipPart()
	{
		if (distFromUFO <= 997f && distFromUFO > 980f)
		{
			GenerateShipPart();
		}
		else if (distFromUFO <= 910f && distFromUFO > 900f)
		{
			GenerateShipPart();
		}
	}

	//here is where all the functions that will cause an object to be spawned are
	//each of them will follow the same rule of randomly selecting one of the elements from the object array
	//it will do this by picking a random int from 0 to the max length of the elements within the object array
	//such as int ran = Random.Range(0, objectArray.Length);
	//it will then instantiate the object at the point that the raycast has hit on the mesh
	//for now at least use Quaternion Identity so there is no rotation with the object and it aligns with the level
	//then it will add one to the placedUFO int so the game knows that an object has been placed
	void GenerateBeachObj()
	{
		int random = Random.Range(0, beachObj.Length);
		Instantiate(beachObj[random], rayHasHit.point, Quaternion.identity);
		placedUFO++;
	}

	void GenerateGrassObj()
	{
		int random = Random.Range(0, grassObj.Length);
		Instantiate(grassObj[random], rayHasHit.point, Quaternion.identity);
		placedUFO++;
	}

	void GenerateCliffObj()
	{
		int random = Random.Range(0, cliffObj.Length);
		Instantiate(cliffObj[random], rayHasHit.point, Quaternion.identity);
		placedUFO++;
	}

	void GenerateCliffTopObj()
	{
		int random = Random.Range(0, cliffTopObj.Length);
		Instantiate(cliffTopObj[random], rayHasHit.point, Quaternion.identity);
		placedUFO++;
	}

	void GenerateMountainObj()
	{
		int random = Random.Range(0, mountainObj.Length);
		Instantiate(mountainObj[random], rayHasHit.point, Quaternion.identity);
		placedUFO++;
	}

	void GenerateSnowObj()
	{

	}

	//this will generate the ship part for the player to collect on the level
	//unlike the other objects, the ship part will float above the ground and will spin in place
	//and then add 1 to the collectthis int so the game knows that a ship part has been place
	void GenerateShipPart()
	{

		//instantiate the ship part at where the raycast hit around -100 on the y axis
		//do this by finding the distance then multiplying by 0.9 to get 90% of the distance and then 
		//using that as the place to spawn it, this may cause it to spawn inside a mountain actually
		//Instantiate(shipPart, rayHasHit.point * 0.9f,  Quaternion.identity);
		//instead just spawn at the point where the raycast hits like a normal object then move it on the y axis
		//using a seperate function once all objects have been placed?
		//ok that didn't work either, let's see if I can do it another way
		//Instantiate(shipPart, rayHasHit.point, Quaternion.identity);

		//ok set up a new vector 3 as the point where the raycast hit
		//shipPartHere = rayHasHit.point;
		//then set up another vector3 that holds all those points together
		//shipPartThere.x = shipPartHere.x ;
		//the y axis needs changing, originally had it at * 0.9f but that spawned them under the map so 
		//must be the wrong way, so * 1.1 should work?
		//shipPartThere.y = shipPartHere.y * 1.5f;
		//shipPartThere.z = shipPartHere.z;
		//Instantiate(shipPart, (shipPartThere.x, shipPartThere.y, shipPartThere.z), Quaternion.identity);
		
		//let's try again
		//instantiate the ship part at the point that it hits, then add to that value a new vector3 with
		//the following values x = 0, y = 15, and z = 0, so essentially just adding on 15 to the height
		//have it set with quaternion identity so it is placed the right way up.
		Instantiate(shipPart, rayHasHit.point + new Vector3(0, 15, 0), Quaternion.identity);
		
		
		collectThis++;
	}

	public void DisableTheUFO()
	{
		theUFO.SetActive(false);
	}



	

	
}
