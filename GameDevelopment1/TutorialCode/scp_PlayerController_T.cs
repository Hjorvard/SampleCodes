using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class scp_PlayerController_T : MonoBehaviour
{
    //------------------------------------------------------
    //-----------------------Scripts------------------------
    //------------------------------------------------------

    private scp_ResourceScript_T resourceScript;

    private scp_TutorialManager tutorialManager;

    private scp_GauntletItem_T gauntletItem;

    //------------------------------------------------------
    //----------------------movement------------------------
    //------------------------------------------------------

    private float WalkSpeed;
    private float RunSpeed;
    public bool isRunning;

    private float MoveX;
    private float MoveY;
    public Vector2 PlayerMovement;
    public Vector2 LastMoveDir;

    private Rigidbody2D PlayerRB;


    //-------------------------------------------------------
    //-----------------------stats---------------------------
    //-------------------------------------------------------

    public float CurHealth;
    public float MaxHealth;

    public bool isInvulnerable;

    public bool hasGauntlet;
    public bool isInteractable;
   

    //-------------------------------------------------------
    //-----------------------combat--------------------------
    //-------------------------------------------------------

    public WhatWeaponIsActive ActiveWeapon;

    private float RaycastLength;
    public LayerMask EnemyLayer;

    public enum WhatWeaponIsActive
    {
        sword,
        spear,
        axe
    }

    private Vector2 pointerInput;
    public Vector2 PointerInput => pointerInput;

    [SerializeField]
    private InputActionReference pointerPosition, attack;

    private scp_weaponParent_T weaponParent_T;

    //-------------------------------------------------------
    //---------------------Gathering-------------------------
    //-------------------------------------------------------

    public float ResourceRayLength = 1f;
    public LayerMask ResourceLayer;

    


    //-------------------------------------------------------
    //------------------------dice---------------------------
    //-------------------------------------------------------

    public GameObject Dice;


    //-------------------------------------------------------
    //---------------------Animations------------------------
    //-------------------------------------------------------


    //-------------------------------------------------------
    //----------------------Functions------------------------
    //-------------------------------------------------------


    private void Awake()
    {
        //set movement stats here
        WalkSpeed = 5f;
        RunSpeed = 10f;
        isRunning = false;
        //get the rigidbody2d attached to this object
        PlayerRB = GetComponent<Rigidbody2D>();

        //set the health
        CurHealth = 10f;
        MaxHealth = 10f;
        //set invulnerable to false
        isInvulnerable = false;

        //set layer masks for handling collisions
        EnemyLayer = LayerMask.GetMask("Enemy");
        ResourceLayer = LayerMask.GetMask("Resource");

        //get the relevant scripts
        resourceScript = FindObjectOfType<scp_ResourceScript_T>();
        tutorialManager = FindObjectOfType<scp_TutorialManager>();
        gauntletItem = FindObjectOfType<scp_GauntletItem_T>();
        weaponParent_T = FindObjectOfType<scp_weaponParent_T>();

        //draw a raycast in the game screen
        Debug.DrawRay(transform.position, Vector2.up, Color.red, ResourceRayLength);

        //set has the gauntlet to false
        hasGauntlet = false;
        //set is interactable to false
        isInteractable = false;

    }


    private void Update()
    {
        GetPlayerInput();
        Debug.DrawRay(transform.position, Vector2.up, Color.red, ResourceRayLength);



    }

    private void FixedUpdate()
    {
        MovePlayer();
        Debug.DrawRay(transform.position, Vector2.up, Color.red, ResourceRayLength);


    }

    private void OnEnable()
    {
        attack.action.performed += PerformAttack;
    }

    private void OnDisable()
    {
        attack.action.canceled -= PerformAttack;
    }



    //-------------------------------------------------------
    //-----------------------Custom--------------------------
    //----------------------Functions------------------------
    //-------------------------------------------------------

    //Function to move the player, call from Fixed Update
    void MovePlayer()
    {
        //if the player is not running set the movement speed to walk speed, if they are running have it as run speed for faster movement
        if (!isRunning)
        {
            PlayerRB.MovePosition(PlayerRB.position + PlayerMovement * WalkSpeed * Time.deltaTime);
        }
        else if (isRunning)
        {
            PlayerRB.MovePosition(PlayerRB.position + PlayerMovement * RunSpeed * Time.deltaTime);
        }
    }


    //function to get the player's inputs, call from update
    void GetPlayerInput()
    {
        //get input from keyboard or controller for the X and Y axis
        MoveX = Input.GetAxisRaw("Horizontal");
        MoveY = Input.GetAxisRaw("Vertical");

        pointerInput = GetPointerInput();

        if ((MoveX == 0 && MoveY == 0) && PlayerMovement.x != 0 || PlayerMovement.y != 0)
        {
            LastMoveDir = PlayerMovement;
        }

        //set the vector2 to a normalised value between the two to prevent faster diagonal movement
        PlayerMovement = new Vector2(MoveX, MoveY).normalized;

        //if the player is holding down shift then change the bool, when they release shift set it back to false to handle sprinting
        if (Input.GetKeyDown(KeyCode.LeftShift))
        {
            isRunning = true;
        }
        if (Input.GetKeyUp(KeyCode.LeftShift))
        {
            isRunning = false;
        }

        //get the input for mining resource
        if (Input.GetKeyDown(KeyCode.Q))
        {
            MineResource();
        }

        if (Input.GetKeyDown(KeyCode.E))
        {
            if (isInteractable)
            {
                gauntletItem.PickupGauntlet();
            }
        }

        weaponParent_T.PointerPosition = pointerInput;


    }

    //function to call when the player is attacking
    void Attack()
    {
        if (ActiveWeapon == WhatWeaponIsActive.axe)
        {

        }
        else if (ActiveWeapon == WhatWeaponIsActive.sword)
        {

        }
        else if (ActiveWeapon == WhatWeaponIsActive.spear)
        {

        }
    }

    //function to take damage from enemy attacks
    //called from an enemy when taking damage
    public void TakeDamage(float DamageToTake)
    {
        CurHealth -= DamageToTake;
    }

    void RollDice()
    {

    }

    //check the weapon the player has equipped
    void CheckWeapon()
    {
        switch (ActiveWeapon)
        {
            case WhatWeaponIsActive.axe:
                break;
            case WhatWeaponIsActive.spear:
                break;
            case WhatWeaponIsActive.sword:
                break;
        }
    }

    void MineResource()
    {
        //if the player does not have the gauntlet
        //this won't be used in the main script
        if (!hasGauntlet)
        {
            return;
        }

        else if (hasGauntlet)
        {
            if (PlayerMovement.y == 0 && PlayerMovement.x == 0)
            {

           
                //if the player was last moving to the right
                if (LastMoveDir == new Vector2(1, 0))
                {
                    Debug.Log("idle right");
                    //fire a raycast to the right of the player, in the direction they are facing
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.right, ResourceRayLength, ResourceLayer);
                    //draw the raycast to see in the editor view
                    Debug.DrawRay(transform.position, Vector2.right, Color.red, RaycastLength);
                    //if the collider hits with something
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }
                //same as above but to the left
                else if (LastMoveDir == new Vector2(-1, 0))
                {
                    Debug.Log("idle left");
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.left, ResourceRayLength, ResourceLayer);
                    Debug.DrawRay(transform.position, Vector2.left, Color.red, RaycastLength);
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }
                //same as above but for upwards
                else if (LastMoveDir == new Vector2(0, 1))
                {
                    Debug.Log("idle up");
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.up, ResourceRayLength, ResourceLayer);
                    Debug.DrawRay(transform.position, Vector2.up, Color.red, RaycastLength);
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }
                //and same again for downwards
                else if (LastMoveDir == new Vector2(0, -1))
                {
                    Debug.Log("idle down");
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.down, ResourceRayLength, ResourceLayer);
                    Debug.DrawRay(transform.position, Vector2.down, Color.red, RaycastLength);
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }
            }
            //however if the player is moving, we use the direction they are currently moving rather than the last direction
            else if (PlayerMovement.y != 0 || PlayerMovement.x != 0)
            {
                //if the player is moving right
                if (PlayerMovement == new Vector2(1, 0))
                {
                    //put in the log the player was moving to the right
                    Debug.Log("Moving Right");
                    //fire a raycast to the right
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.right, ResourceRayLength, ResourceLayer);
                    //if the raycast hits
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }

                }
                //same as above but for when moving left
                else if (PlayerMovement == new Vector2(-1, 0))
                {
                    Debug.Log("Moving Left");
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.left, ResourceRayLength, ResourceLayer);
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }
                //same as above but for when moving up
                else if (PlayerMovement == new Vector2(0, 1))
                {
                    Debug.Log("Moving up");
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.up, ResourceRayLength, ResourceLayer);
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }
                //same as above but for when moving down
                else if (PlayerMovement == new Vector2(0, -1))
                {
                    Debug.Log("Moving down");
                    RaycastHit2D hit = Physics2D.Raycast(transform.position, Vector2.down, ResourceRayLength, ResourceLayer);
                    if (hit.collider != null)
                    {
                        Debug.Log("Hit Successful");
                        hit.collider.SendMessage("TakeDamage");
                    }
                }


            }
        }
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.collider.tag == "StoneRes")
        {
            tutorialManager.IncreaseStone();
            Destroy(collision.collider.gameObject);

        }

        /*if (collision.collider.tag == "Gauntlet")
        {
            isInteractable = true;
        }
        */
    }

    private void OnCollisionExit2D(Collision2D collision)
    {

    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.tag == "Gauntlet")
        {
            isInteractable = true;
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.tag == "Gauntlet")
        {
            isInteractable = false;
        }
    }

    public void GetGauntlet()
    {
        hasGauntlet = true;
    }

    private Vector2 GetPointerInput()
    {
        Vector3 mousePosition = pointerPosition.action.ReadValue<Vector2>();
        mousePosition.z = Camera.main.nearClipPlane;
        return Camera.main.ScreenToWorldPoint(mousePosition);
    }

    private void PerformAttack(InputAction.CallbackContext obj)
    {
        weaponParent_T.Attack();
    }





}
