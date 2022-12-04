using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class scp_weaponParent_T : MonoBehaviour
{

    public Vector2 PointerPosition { get; set; }
    public bool isAttacking { get; private set; }


    [SerializeField]
    private Animator animator;
    private float Delay = 0.3f;
    private bool canAttack;

    public Transform circleOrigin;
    public float circleRadius;


    private void Awake()
    {
        animator = GetComponentInChildren<Animator>();
        canAttack = true;
    }

    private void Update()
    {
        GetPointerPosition();
    }

    public void Attack ()
    {
        if (!canAttack)
        {
            return;
        }
        animator.SetTrigger("Attack");
        canAttack = false;
        isAttacking = true;
        StartCoroutine(AttackPause());
    }

    private IEnumerator AttackPause()
    {
        yield return new WaitForSeconds(Delay);
        canAttack = true;
    }

    public void SetIsAttacking()
    {
        isAttacking = false;
    }

    public void GetPointerPosition()
    {

        if (isAttacking)
        {
            return;
        }

        transform.right = (PointerPosition - (Vector2)transform.position).normalized;

        Vector2 direction = (PointerPosition - (Vector2)transform.position).normalized;
        transform.right = direction;

        Vector2 scale = transform.localScale;
        if (direction.x < 0)
        {
            scale.y = -1;
        }
        else if (direction.x > 0)
        {
            scale.y = 1;
        }

        transform.localScale = scale;
    }

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        Vector3 position = circleOrigin == null ? Vector3.zero : circleOrigin.position;
        Gizmos.DrawWireSphere(position, circleRadius);
    }

    public void DetectColliders()
    {
        foreach (Collider2D collider in Physics2D.OverlapCircleAll(circleOrigin.position, circleRadius))
        {
            Debug.Log(collider.name);
        }
    }
}
