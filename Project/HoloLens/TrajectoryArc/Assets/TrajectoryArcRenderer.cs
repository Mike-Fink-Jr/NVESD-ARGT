using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(LineRenderer))]
public class TrajectoryArcRenderer : MonoBehaviour {

    LineRenderer lr;

    public float velocity;
    public float angle;
    public int resolution = 10; // Higher resolution would give higher quality arc
    public float initHeight;

    float grav; // force of gravity on the y axis
    float radianAngle;


    private void Awake(){
        lr = GetComponent<LineRenderer>();
        grav = Mathf.Abs(Physics2D.gravity.y);
    }

    private void OnValidate(){
        //Check that lr is not null and that the program is running
        if (lr != null && Application.isPlaying){
            RenderArc();
        }
    }

    // Use this for initialization
    void Start () {
        RenderArc();
	}

    //Populate LineRenderer with settings
    void RenderArc(){
        lr.SetVertexCount(resolution + 1);
        lr.SetPositions(CalculateArcArray());
    }

    //make an array of Vector 3 positions for the arc
    Vector3[] CalculateArcArray() {
        Vector3[] arcArray = new Vector3[resolution + 1];

        radianAngle = Mathf.Deg2Rad * angle;
        float maxDistance = (velocity * velocity * Mathf.Sin(2 * radianAngle)) / grav; //Eqation for max distance

        for (int i = 0; i <= resolution; i++){
            float t = (float)i / (float)resolution;     //Evenly spaces points out along array 
            arcArray[i] = CalculateArcPoint(t, maxDistance);
        }

        return arcArray;

    }

    //Calculate height and distance of each vertex
    Vector3 CalculateArcPoint(float t, float maxDistance){
        float x = t * maxDistance;
        float y = initHeight + (x * Mathf.Tan(radianAngle)) - ((grav * x * x) / (2 * velocity * velocity * Mathf.Cos(radianAngle) * Mathf.Cos(radianAngle)));      //Height at x formula
        return new Vector3(x, y);
    }
}
