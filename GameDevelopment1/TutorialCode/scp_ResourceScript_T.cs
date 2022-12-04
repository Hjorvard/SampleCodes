using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class scp_ResourceScript_T : MonoBehaviour
{
    public int ResHealth;
    public int ResMaxHealth;

    public GameObject ResourcePickUp;
    private float DropDistance = 1f;


    // Start is called before the first frame update
    void Start()
    {

        ResHealth = 1;
        ResMaxHealth = 1;

       // use this in the proper version not the Tutorial
       // ResMaxHealth = Random.Range(1, 4);
       // ResHealth = ResMaxHealth;
    }

    // Update is called once per frame
    void Update()
    {
        if (ResHealth <= 0)
        {
            Destroy(this.transform.gameObject);
        }
    }

    public void TakeDamage()
    {

        ResHealth -= 1;
        Vector2 position = transform.position;
        position.x += DropDistance * UnityEngine.Random.value - DropDistance / 2;
        position.y += DropDistance * UnityEngine.Random.value - DropDistance / 2;
        GameObject drop = Instantiate(ResourcePickUp);
        drop.transform.position = position;

    }
}
