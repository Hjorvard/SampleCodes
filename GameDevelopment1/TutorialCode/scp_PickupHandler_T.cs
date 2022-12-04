using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class scp_PickupHandler_T : MonoBehaviour
{
    public Transform ThePlayer;
    private float PickupDistance = 3f;
    private float PickupSpeed = 5f;

    public scp_TutorialManager TM;

    private void Awake()
    {
        TM = FindObjectOfType<scp_TutorialManager>();

        ThePlayer = TM.ThePlayer.transform;
    }

    private void Update()
    {
        float distance = Vector3.Distance(transform.position, ThePlayer.position);

        if (distance > PickupDistance)
        {
            return;
        }
        transform.position = Vector3.MoveTowards(transform.position, ThePlayer.position, PickupSpeed * Time.deltaTime);
    }
}
