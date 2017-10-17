using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(MeshFilter))]
public class TrajectoryArcMesh : MonoBehaviour
{

    Mesh mesh;
    public float meshWidth;

    public float velocity;
    public float angle;
    public int resolution = 10; // Higher resolution would give higher quality arc
    public float initHeight;

    float grav; // force of gravity on the y axis
    float radianAngle;


    private void Awake(){
        mesh = GetComponent<MeshFilter>().mesh;
        grav = Mathf.Abs(Physics2D.gravity.y);
    }

    private void OnValidate(){
        //Check that lr is not null and that the program is running
        if (mesh != null && Application.isPlaying){
            MakeArcMesh(CalculateArcArray());
        }
     }

    // Use this for initialization
    void Start(){
        MakeArcMesh(CalculateArcArray());
    }

    void MakeArcMesh(Vector3[] arcVerts){
        mesh.Clear();               //Clears any previous points
        Vector3[] vertices = new Vector3[(resolution + 1) * 2];
        int[] triangles = new int[resolution * 6 * 2];              // 6 triangle vertices for top and underneath of arc

        for (int i = 0; i <= resolution; i++){
            // set vertices
            vertices[i * 2] = new Vector3(meshWidth * 0.5f, arcVerts[i].y, arcVerts[i].x);            // x is width, y is height, z is the distance
            vertices[i * 2 + 1] = new Vector3(meshWidth * -0.5f, arcVerts[i].y, arcVerts[i].x);

            // set triangles
            if(i != resolution){
                // top half
                triangles[i * 12] = i * 2;
                triangles[i * 12 + 1] = triangles[i * 12 + 4] = i * 2 + 1;
                triangles[i * 12 + 2] = triangles[i * 12 + 3] = (i + 1) * 2;
                triangles[i * 12 + 5] = (i + 1) * 2 + 1;

                // bottom half
                triangles[i * 12 + 6] = i * 2;
                triangles[i * 12 + 7] = triangles[i * 12 + 10] = (i + 1) * 2;
                triangles[i * 12 + 8] = triangles[i * 12 + 9] = i * 2 + 1;
                triangles[i * 12 + 11] = (i + 1) * 2 + 1;
            }

            mesh.vertices = vertices;
            mesh.triangles = triangles;
        }
    }

    //make an array of Vector 3 positions for the arc
    Vector3[] CalculateArcArray()
    {
        Vector3[] arcArray = new Vector3[resolution + 1];

        radianAngle = Mathf.Deg2Rad * angle;
        float maxDistance = (velocity * velocity * Mathf.Sin(2 * radianAngle)) / grav; //Eqation for max distance

        for (int i = 0; i <= resolution; i++)
        {
            float t = (float)i / (float)resolution;     //Evenly spaces points out along array 
            arcArray[i] = CalculateArcPoint(t, maxDistance);
        }

        return arcArray;

    }

    //Calculate height and distance of each vertex
    Vector3 CalculateArcPoint(float t, float maxDistance)
    {
        float x = t * maxDistance;
        float y = initHeight + (x * Mathf.Tan(radianAngle)) - ((grav * x * x) / (2 * velocity * velocity * Mathf.Cos(radianAngle) * Mathf.Cos(radianAngle)));      //Height at x formula 
        return new Vector3(x, y);
    }
}
