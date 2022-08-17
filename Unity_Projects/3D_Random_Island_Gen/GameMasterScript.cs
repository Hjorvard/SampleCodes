using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class GameMasterScript : MonoBehaviour 
{

	static GameMasterScript instance;

	//--------------------------------------------------------------UI------------------------------------------------------------------------------

	//ints
	public int scoreInt;
	public int collectiblesInt;
	public  int timeInt;

	//bool for handling the time remaining
	public bool countdown;
	//bool that stops the map annoyingly generating over and over again
	public bool stopGenPls;
	

	//text to use with the UI
	public Text score;
	public Text collectibles;
	public Text timeLeft;

	//reference to the Raycasting UFO
	public GameObject theUFOObj;
	public RaycastingUFO theUFONow;

	void Awake()
	{
		//if there's already a Game Master object when the level resets, destroy it instantly. If there isn't
		//then set the first GM object to instance, and do not destroy it on scene changes or reloads
		if (instance == null)
		{
			DontDestroyOnLoad(this);
			instance = this;	
		}
		else
		{
			DestroyImmediate(this);
		}

		//set the score and time ints on the first awake
		scoreInt = 0;
		timeInt = 300;
		//set stop gen to true on awake to stop the map regenerating over and over again
		stopGenPls = true;
		
	}

	void Update()
	{
		//displays the UI as text explaining what it is, plus a numeric value based on stored variables
		score.text = "Score: " + scoreInt;
		collectibles.text = "Collectibles Left: " + collectiblesInt; 
		timeLeft.text = "Time Remaining: " + timeInt;

		//reload the scene creating an entire new map upon pressing F5
		if (Input.GetKeyDown(KeyCode.F5))
		{
			//call the reset score function and then reload the current scene
			ResetScore();
			SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
		}

		//when the player collects all the collectibles
		if (collectiblesInt <= 0 && stopGenPls == false)
		{
			//call the next level function to generate the next level
			NextLevel();
		}

		if ( timeInt <= 0)
		{
			countdown = false;
			TimeRanOut();
		}

	}

	//called from the player script when the player collides with a collectible
	public void YouScored()
	{
		//adds to the score and reduces the number of remaining collectibles in the UI by 1
		scoreInt = scoreInt + 100;
		collectiblesInt = collectiblesInt - 1;

	}

	//sets up the start information for the level, the amount of collectibles
	//also starts the coroutine that counts down time and makes the map able to generate afresh
	//when the player has either died, pressed f5 or completed the current level
	public void InitialCollect()
	{
		collectibles.text = "Collectibles Left: " + theUFONow.collectThis;
		collectiblesInt = theUFONow.collectThis;
		countdown = true;
		StartCoroutine (LevelTimer());
		stopGenPls = false;
		
	}

	//just a script that is called early on in another script that allows this script to get
	//the raycastingUFO script from the UFO object
	public void GetUFO()
	{
		theUFOObj = GameObject.FindGameObjectWithTag("UFO");
		theUFONow = (RaycastingUFO) theUFOObj.GetComponent(typeof(RaycastingUFO));

	}

	//the enum that is used to countdown time, I used this instead of using time.deltatime since it
	//kept having issues with pausing and reloading scenes, this works just as well and so far proved more reliable
	public IEnumerator LevelTimer()
	{
		while (countdown == true)
		{
			yield return new WaitForSeconds(1);
			timeInt--;
		}
		
	}

	//pauses the timer, and sets the map gen stopper to true so that the map will not keep regenerating once
	//it reloads the scene since this object is not deleted.
	//adds to the score by taking the time remaining and multiplying it by 10 and then adding it to current score.
	//also adds 31 to the timeint giving the player an addition 30 seconds (it keeps taking 1 second off straight away
	//that's why I used 31 rather than 30) to complete the next level.
	public void NextLevel()
	{
		stopGenPls = true;
		countdown = false;
		scoreInt = scoreInt + (timeInt * 10);
		timeInt = timeInt + 31;
		SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
	}

	//resets the score, timer, and disables the countdown
	//called several times from other functions
	public void ResetScore()
	{
		countdown = false;
		scoreInt = 0;
		timeInt = 301;
	}

	//takes the player to the death screen.
	public void YouDied()
	{
		Cursor.lockState = CursorLockMode.None;
		ResetScore();
		SceneManager.LoadScene("You Died");
	}

	//takes the player to the score screen
	//adds 6 to the time int to stop it causing reload issues.
	public void TimeRanOut()
	{
		timeInt = 6;
		Cursor.lockState = CursorLockMode.None;
		SceneManager.LoadScene("You Ran Out of Time");
	}
	
}
