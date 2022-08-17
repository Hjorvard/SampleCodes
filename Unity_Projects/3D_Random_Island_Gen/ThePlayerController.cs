using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ThePlayerController : MonoBehaviour 
{

	//floats
	//how fast the player will walk
	public float speed = 75;
	//the force that the player will push off from the ground when jumping
	public float jumpPower = 150;
	//custom gravity value to be set, not actual gravity used by Unity
	public float gravity = 100;
	//the velocity that the player moves up and down on the y axis (jumping)
	public float yVelocity;

	//gets the character controller set to the player
	public CharacterController thisIsThePlayer;

	//gets a reference to the game master script for keeping the score.
	public GameObject theGM;
	public GameMasterScript GMScript;
	

	void Start () 
	{
		//set the floats here here just in case something happens and they change by accident, disable this whilst testing
		speed = 75;
		jumpPower = 150;
		gravity = 100;

		//disable the mouse cursor in play mode
		Cursor.lockState = CursorLockMode.Locked;

		//get the object tagged with GM
		theGM = GameObject.FindGameObjectWithTag("GM");
		GMScript = (GameMasterScript) theGM.GetComponent(typeof(GameMasterScript));

	}
	

	void Update () 
	{
		//if the player is holding down the fire3 button, which in this case is the left shift key
		//then increase the speed by 50% by doing speed times by 1.5, disabled for now, needs fixing before being used
		//if (Input.GetButton("Fire3"))
		//	{
		//	speed = speed * 1.5f;
		//}

		//the player movement, this does not include the mouse looking movement, just the player movement

		//set 2 floats, one for movement on the x axis and another for movement on the y axis
		//first get a float for the direction of the movement for moving forwards and backwards
		//do this by getting an input which is defined in the project settings for the game
		// in this case horizontal (x axis) and vertical (z axis) then multiply it by the speed float
		float backAndForwards = Input.GetAxis("Vertical") * speed;
		float straffing = Input.GetAxis("Horizontal") * (speed * 0.75f);

		//the floats are the same as *= Time.deltaTime, this means the floats are equal to the float * Time.deltaTime, this 
		//will cause the movement to stay in time with the update, and make movement appear smoother than it would otherwise. 
		//Time.deltaTime is the time between the 2 update frames, allowing for everything to remain largely consistent
		backAndForwards *= Time.deltaTime;
		straffing *= Time.deltaTime;

		//now make the values actually do something.
		//using transform.translate to move the object along the relevant axis, (x, y, z)
		//we feed into the transform.translate our x axis movement which in this case is the float straffing
		//then nothing for the movement along the y axis, and for the z axis we use backwardsandforwards
		//this causes the character to move according to the set values when the player tries to move
		//along either the x or the z axis and presses a button to move.
		transform.Translate(straffing, 0, backAndForwards);
		
		//if the player controller is not jumping and is touching an object causing it to be grounded
		if (thisIsThePlayer.isGrounded)
		{
			//the y velocity is set to -the gravity * the time between frames
			//this causes the player to slowly move down the y axis with a small force, different from the
			//large force pulling the player down when jumping.
			yVelocity = -gravity * Time.deltaTime;

			//and if the player is grounded, and then presses the jump button that is set in settings to
			//be the space bar then we will want the player to jump
			if (Input.GetButtonDown("Jump"))
			{
				//set the y velocity to the jump force for use in a second
				yVelocity = jumpPower;
							
			}
		}
		//but if the player isn't grounded
		else 
		{
			//set the y velocity to y velocity minus (gravity * the time between update frames)
			//this will increase the longer the player is not grounded causing the player to fall more realistically
			yVelocity -= gravity * Time.deltaTime;
		}

		//the vector3 moving y now takes in nothing on the x and z axis but takes in the y velocity on the y axis.
		//So when the player presses the jump button the player will move upwards on the y axis slowing as they reach
		//the peak jump height before falling back down at an increasing rate
		Vector3 movingY = new Vector3 (0, yVelocity, 0);
		//when the character controllermoves on the y axis
		thisIsThePlayer.Move(movingY * Time.deltaTime);

	
		//start by making it so that by pressing escape the player can regain mouse control
		//make sure this is the last bit of code here in update, nothing under it works otherwise
		//caused me no end of problems until I realised that this  was why nothing worked!
		if (Input.GetKeyDown("Escape"))
		{
			Cursor.lockState = CursorLockMode.None;
		} 

	}

	//when the player collider connects with the collectible collider
	void OnTriggerEnter(Collider shipPart)
	{
		//if the collision is with something that is tagged with collectible...
		if (shipPart.gameObject.CompareTag ("collectible"))
		{
			GMScript.YouScored();
			//...then we will disable the object rather than delete it.
			shipPart.gameObject.SetActive (false);
			
			

		}
	}

	
}
