/* ******************************************************************************
* Real Flow Mesh Geometry File
*
* $RCSfile: real_flow_mesh.C,v $
*
* Description : Implementation for the RealFlow_Mesh_File class
*
* $Revision: 1.2 $
*
* $Source: /dca/cvsroot/houdini/SOP_RealFlow/real_flow_mesh.C,v $
*
* $Author: mstory $
*
* See Change History at the end of the file.
*
*    Digital Cinema Arts (C) 2005
*
* This work is licensed under the Creative Commons Attribution-ShareAlike 2.5 License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/2.5/ or send a letter to
* Creative Commons, 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
*
***************************************************************************** */

#ifndef __real_flow_mesh_C__
#define __real_flow_mesh_C__

namespace dca
{

   /* ******************************************************************************
   *  Function Name : RF_Mesh_Exception()
   *
   *  Description : Exception class for the Real Flow Mesh File Object
   *
   *
   *  Input Arguments : std::string msg
   *
   *  Return Value : None
   *
   ***************************************************************************** */

   RF_Mesh_Exception::RF_Mesh_Exception(std::string msg)
   {

      std::cout << "RF_Mesh_Exception: in constructor ... " << std::endl;

      e_msg = msg;
   };

   /* ******************************************************************************
   *  Function Name : RealFlow_Mesh_File()
   *
   *  Description : Constructor for the Real Flow Mesh File Object
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : None
   *
   ***************************************************************************** */
   RealFlow_Mesh_File::RealFlow_Mesh_File()
   {
      int i;

      mesh_header.ID_code = 0;
      mesh_header.version = 0;
      mesh_header.code = 0;

      mesh_vertex_data.num_vertices = 0;
      mesh_vertex_data.X = 0;
      mesh_vertex_data.Y = 0;
      mesh_vertex_data.Z = 0;

      mesh_face_data.num_faces = 0;
      mesh_face_data.vertex[0] = 0;
      mesh_face_data.vertex[1] = 0;
      mesh_face_data.vertex[2] = 0;

      mesh_tex_data.code = 0;
      mesh_tex_data.num_fluids = 0;
      mesh_tex_data.U = 0;
      mesh_tex_data.V = 0;
      mesh_tex_data.W = 0;

      for(i = 0; i < MAX_NUM_FLUIDS; i++)
         mesh_tex_data.texture_weight[i] = 0;

      mesh_vel_data.code = 0;
      mesh_vel_data.X = 0;
      mesh_vel_data.Y = 0;
      mesh_vel_data.Z = 0;

   }



   /* ******************************************************************************
   *  Function Name : ~RealFlow_Mesh_File()
   *
   *  Description : Destructor for the Real Flow Mesh File Object
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : None
   *
   ***************************************************************************** */
   RealFlow_Mesh_File::~RealFlow_Mesh_File()
   {

   }



   /* ******************************************************************************
   *  Function Name : openMeshFile()
   *
   *  Description : Open the Real Flow mesh file
   *
   *
   *  Input Arguments : char *file_name
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   int RealFlow_Mesh_File::openMeshFile(const char * file_name, int mode)
   {

#ifdef DEBUG
      std::cout << "RealFlow_Mesh_File::openMeshFile() - Opening Real Flow Mesh file: " << file_name << std::endl << std::endl;
#endif
      try {

            if(mode == RF_FILE_READ) {

                  RFMeshifstream.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
                  RFMeshifstream.open((const char *) file_name, std::ios::in | std::ios::binary);
#ifdef DEBUG
                  std::cerr << "RealFlow_Mesh_File::openMeshFile(): Opened Real Flow mesh file for reading" << std::endl;
#endif
                  return 0;
               }

            else {

                  RFMeshofstream.exceptions(std::ofstream::eofbit | std::ofstream::failbit | std::ofstream::badbit);
                  RFMeshofstream.open((const char *) file_name, std::ios::out | std::ios::binary);
#ifdef DEBUG
                  std::cerr << "RealFlow_Mesh_File::openMeshFile(): Opened Real Flow mesh file for writng" << std::endl;
#endif
                  return 0;
               }

         }

      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::openMeshFile(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            return 1;
         }

   }



   /* ******************************************************************************
   *  Function Name : readMeshFileHeader()
   *
   *  Description : Read the Real Flow mesh file header
   *
   *
   *  Input Arguments : int *hdr_status
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   int RealFlow_Mesh_File::readMeshFileHeader(int * hdr_status)
   {

#ifdef DEBUG
      std::cout << "RealFlow_Mesh_File::readMeshFileHeader():Reading Real Flow Mesh file header" << std::endl << std::endl;
#endif

      try {
            // Read the ID_code
            RFMeshifstream.read((char *) &mesh_header.ID_code, sizeof(unsigned int));

            // If this is not a Real Flow Mesh file, return
            if(mesh_header.ID_code != 0xDADADADA) {
                  std::cerr << "RealFlow_Mesh_File::readMeshFileHeader(): Real Flow Mesh ID code is incorrect = "
                            << std::hex << mesh_header.ID_code << std::dec <<  std::endl;
                  RFMeshifstream.clear();
                  RFMeshifstream.close();
                  *hdr_status = -1;
                  return 1;
               }

            // read the version
            RFMeshifstream.read((char *) &mesh_header.version, sizeof(unsigned int));

            // Read the "geometry" code
            RFMeshifstream.read((char *) &mesh_header.code, sizeof(unsigned int));

            // Read the number of vertices
            RFMeshifstream.read((char *) &mesh_vertex_data.num_vertices, sizeof(int));

#ifdef DEBUG
            std::cout << "ID_code = " << std::hex << mesh_header.ID_code << std::endl;
            std::cout << "version = " << std::dec << mesh_header.version << std::endl;
            std::cout << "geometry code = " << std::hex << mesh_header.code << std::dec << std::endl;
            std::cout << "num_vertices = " << mesh_vertex_data.num_vertices << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshFileHeader(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : readMeshVertexData()
   *
   *  Description : Read the Real Flow mesh file vertex data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshVertexData()
   {

      try {
            // Read the number of vertices
            RFMeshifstream.read((char *) &mesh_vertex_data.Z, sizeof(float));
            RFMeshifstream.read((char *) &mesh_vertex_data.Y, sizeof(float));
            RFMeshifstream.read((char *) &mesh_vertex_data.X, sizeof(float));

#ifdef DEBUG
            std::cout << "mesh_vertex_data = " << mesh_vertex_data.X <<  " "
                      << mesh_vertex_data.Y <<  " " << mesh_vertex_data.Z << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshVertexData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : readMeshNumFaces()
   *
   *  Description : Read the Real Flow mesh file number of faces
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshNumFaces()
   {

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::readMeshNumFaces() - Reading Mesh number of faces" << std::endl;
#endif

      try {
            // Read the number of faces
            RFMeshifstream.read((char *) &mesh_face_data.num_faces, sizeof(int));

#ifdef DEBUG
            std::cout << "mesh_face_data.num_faces = " << mesh_face_data.num_faces << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshNumFaces(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : readMeshFaceData()
   *
   *  Description : Read the Real Flow mesh file face data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshFaceData()
   {

      try {
            // Read the face data
            RFMeshifstream.read((char *) &mesh_face_data.vertex[2], sizeof(int));
            RFMeshifstream.read((char *) &mesh_face_data.vertex[1], sizeof(int));
            RFMeshifstream.read((char *) &mesh_face_data.vertex[0], sizeof(int));

#ifdef DEBUG
            std::cout << "mesh_face_data.vertex = " << mesh_face_data.vertex[0] << " "
                      << mesh_face_data.vertex[1] <<  " " << mesh_face_data.vertex[2] << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshFaceData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : readMeshChunkCode()
   *
   *  Description : Read the next chunk code
   *
   *
   *  Input Arguments : unsigned int * code
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshChunkCode(unsigned int * code)
   {

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::readMeshChunkCode() - Reading Mesh chunk code" << std::endl << std::endl;
#endif

      try {
            mesh_tex_data.code = 0;
            mesh_vel_data.code = 0;

            // Read the chunk data
            RFMeshifstream.read((char *) code, sizeof(unsigned int));

            if(*code == 0xCCCCCC00) {
                  // Found texture code
                  mesh_tex_data.code = 0xCCCCCC00;
               }
            else
               if(*code == 0xCCCCCC11) {
                     // Found velocity code
                     mesh_vel_data.code = 0xCCCCCC11;
                  }

#ifdef DEBUG
            std::cout << "RealFlow_Mesh_File::readMeshChunkCode() - mesh chunk code  = " << std::hex << *code << std::dec << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshChunkCode(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }




   /* ******************************************************************************
   *  Function Name : readMeshNumFluids()
   *
   *  Description : Read the Real Flow mesh file number of fluids
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshNumFluids()
   {

#ifdef DEBUG
      std::cout << std::endl << " - Reading Mesh number of fluids" << std::endl << std::endl;
#endif

      try {
            // Read the number of fluids
            RFMeshifstream.read((char *) &mesh_tex_data.num_fluids, sizeof(int));

#ifdef DEBUG
            std::cout << "mesh_tex_data.num_fluids = " << mesh_tex_data.num_fluids << std::endl << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshNumFluids(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : readMeshTextureData()
   *
   *  Description : Read the Real Flow mesh file texture data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshTextureData()
   {
      int     i;
      float   a_float, texture_weight_sum;

      try {
            texture_weight_sum = 0;

            // For each fluid, calculate the contribution of each to the "texture weight".
            for(i = 0; i <  mesh_tex_data.num_fluids - 1; i++) {
                  RFMeshifstream.read((char *) &a_float, sizeof(float));
                  mesh_tex_data.texture_weight[i]  = a_float;
                  texture_weight_sum += mesh_tex_data.texture_weight[i];

#ifdef DEBUG
                  std::cout << "mesh_tex_data.texture_weight[i] = " << mesh_tex_data.texture_weight[i] << std::endl;
#endif
               }
            mesh_tex_data.texture_weight[mesh_tex_data.num_fluids - 1] = (1.0 - texture_weight_sum);

            // Read texture coordinates
            RFMeshifstream.read((char *) &mesh_tex_data.W, sizeof(float));
            RFMeshifstream.read((char *) &mesh_tex_data.V, sizeof(float));
            RFMeshifstream.read((char *) &mesh_tex_data.U, sizeof(float));

#ifdef DEBUG
            std::cout << "mesh_tex_data = " << mesh_tex_data.U << " "
                      << mesh_tex_data.V << " " << mesh_tex_data.W << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshTextureData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : readMeshVelocityData()
   *
   *  Description : Read the Real Flow mesh file velocity data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshVelocityData()
   {

      try {
            // Read the number of fluids
            RFMeshifstream.read((char *) &mesh_vel_data.Z, sizeof(float));
            RFMeshifstream.read((char *) &mesh_vel_data.Y, sizeof(float));
            RFMeshifstream.read((char *) &mesh_vel_data.X, sizeof(float));

#ifdef DEBUG
            std::cout << "mesh_vel_data = " << mesh_vel_data.X << "\t"
                      << mesh_vel_data.Y << "\t" << mesh_vel_data.Z << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshVelocityData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : readMeshFileEOF()
   *
   *  Description : Read the Real Flow mesh file end of file marker
   *
   *  This method returns 1 if there's a file error
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::readMeshFileEOF()
   {
      unsigned int eof_code;

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::readMeshFileEOF() - Reading mesh file end of file marker" << std::endl << std::endl;
#endif

      try {
            // Read the number of fluids
            RFMeshifstream.read((char *) &eof_code, sizeof(unsigned int));

            // If there's no eof valid marker, return.
            if(eof_code != 0xDEDEDEDE) {
                  std::cerr << "RealFlow_Mesh_File::readMeshFileEOF(): Invalid EOF marker" << std::endl;
#ifdef DEBUG
                  std::cout << "Invalid EOF marker: " << eof_code << std::endl;
#endif
                  return 1;
               }
         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::readMeshFileEOF(): EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            RFMeshifstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : writeMeshFileHeader()
   *
   *  Description : Write the Real Flow mesh file header
   *
   *
   *  Input Arguments :
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   int RealFlow_Mesh_File::writeMeshFileHeader()
   {

#ifdef DEBUG
      std::cout << "RealFlow_Mesh_File::writeMeshFileHeader():Writing Real Flow Mesh file header" << std::endl << std::endl;
#endif

      try {
            // write the ID_code
            RFMeshofstream.write((char *) &mesh_header.ID_code, sizeof(unsigned int));
            // write the version
            RFMeshofstream.write((const char *) &mesh_header.version, sizeof(unsigned int));
            // write the "geometry" code
            RFMeshofstream.write((const char *) &mesh_header.code, sizeof(unsigned int));
            // write the number of vertices
            RFMeshofstream.write((const char *) &mesh_vertex_data.num_vertices, sizeof(int));

#ifdef DEBUG
            std::cout << "ID_code = " << std::hex << mesh_header.ID_code << std::endl;
            std::cout << "version = " << std::dec << mesh_header.version << std::endl;
            std::cout << "geometry code = " << std::hex << mesh_header.code << std::dec << std::endl;
            std::cout << "num_vertices = " << mesh_vertex_data.num_vertices << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshFileHeader(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : writeMeshVertexData()
   *
   *  Description : Write the Real Flow mesh file vertex data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshVertexData()
   {

      try {
            // Write the number of vertices
            RFMeshofstream.write((char *) &mesh_vertex_data.Z, sizeof(float));
            RFMeshofstream.write((char *) &mesh_vertex_data.Y, sizeof(float));
            RFMeshofstream.write((char *) &mesh_vertex_data.X, sizeof(float));

#ifdef DEBUG
            std::cout << "RealFlow_Mesh_File::writeMeshVertexData(): mesh_vertex_data = " << mesh_vertex_data.X <<  " "
                      << mesh_vertex_data.Y <<  " " << mesh_vertex_data.Z << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshVertexData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : writeMeshNumFaces()
   *
   *  Description : Write the Real Flow mesh file number of faces
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshNumFaces()
   {

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::writeMeshNumFaces(): Writing Mesh number of faces" << std::endl;
#endif

      try {
            // Write the number of faces
            RFMeshofstream.write((char *) &mesh_face_data.num_faces, sizeof(int));

#ifdef DEBUG
            std::cout << "RealFlow_Mesh_File::writeMeshNumFaces(): mesh_face_data.num_faces = " << mesh_face_data.num_faces << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshNumFaces(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : writeMeshFaceData()
   *
   *  Description : Write the Real Flow mesh file face data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshFaceData()
   {

      try {
            // Write the face data
            RFMeshofstream.write((char *) &mesh_face_data.vertex[2], sizeof(int));
            RFMeshofstream.write((char *) &mesh_face_data.vertex[1], sizeof(int));
            RFMeshofstream.write((char *) &mesh_face_data.vertex[0], sizeof(int));

#ifdef DEBUG
            std::cout << "mesh_face_data.vertex = " << mesh_face_data.vertex[0] << " "
                      << mesh_face_data.vertex[1] <<  " " << mesh_face_data.vertex[2] << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshFaceData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : writeMeshTextureChunkCode()
   *
   *  Description : Write the next texture chunk code
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshTextureChunkCode()
   {

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::writeMeshTextureChunkCode() - Writing Mesh texture chunk code" << std::endl << std::endl;
#endif

      try {
            // Write the chunk data
            RFMeshofstream.write((char *) &mesh_tex_data.code, sizeof(unsigned int));

#ifdef DEBUG
            std::cout << "texture chunk code  = " << std::hex <<  mesh_tex_data.code << std::dec << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshTextureChunkCode(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : writeMeshVelocityChunkCode()
   *
   *  Description : Write the next velocity chunk code
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshVelocityChunkCode()
   {

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::writeMeshVelocityChunkCode() - Reading Mesh velocity chunk code" << std::endl << std::endl;
#endif

      try {
            // Write the chunk data
            RFMeshofstream.write((char *) &mesh_vel_data.code, sizeof(unsigned int));

#ifdef DEBUG
            std::cout << "velocity chunk code  = " << std::hex << mesh_vel_data.code << std::dec << std::endl << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshVelocityChunkCode(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : writeMeshNumFluids()
   *
   *  Description : Write the Real Flow mesh file number of fluids
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshNumFluids()
   {

#ifdef DEBUG
      std::cout << std::endl << " - Reading Mesh number of fluids" << std::endl << std::endl;
#endif

      try {
            // Write the number of fluids
            RFMeshofstream.write((char *) &mesh_tex_data.num_fluids, sizeof(int));

#ifdef DEBUG
            std::cout << "mesh_tex_data.num_fluids = " << mesh_tex_data.num_fluids << std::endl << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshNumFluids(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : writeMeshTextureData()
   *
   *  Description : Write the Real Flow mesh file texture data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshTextureData()
   {
      int     i;
      float   a_float, texture_weight_sum;

      try {

            // If more than one fluid, for each fluid, calculate the contribution of each to the "texture weight".
            if(mesh_tex_data.num_fluids > 1) {
                  texture_weight_sum = 0;
                  for(i = 0; i <  mesh_tex_data.num_fluids - 1; i++) {
                        RFMeshofstream.write((char *) &a_float, sizeof(float));
                        mesh_tex_data.texture_weight[i]  = a_float;
                        texture_weight_sum += mesh_tex_data.texture_weight[i];
                        mesh_tex_data.texture_weight[mesh_tex_data.num_fluids - 1] = (1.0 - texture_weight_sum);

#ifdef DEBUG
                        std::cout << "mesh_tex_data.texture_weight[i] = " << mesh_tex_data.texture_weight[i] << std::endl;
#endif
                     }
               }

            // Write texture coordinates
            RFMeshofstream.write((char *) &mesh_tex_data.W, sizeof(float));
            RFMeshofstream.write((char *) &mesh_tex_data.V, sizeof(float));
            RFMeshofstream.write((char *) &mesh_tex_data.U, sizeof(float));

#ifdef DEBUG
            std::cout << "mesh_tex_data = " << mesh_tex_data.U << " "
                      << mesh_tex_data.V << " " << mesh_tex_data.W << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshTextureData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }


   /* ******************************************************************************
   *  Function Name : writeMeshVelocityData()
   *
   *  Description : Write the Real Flow mesh file velocity data
   *
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshVelocityData()
   {

      try {
            // Write the number of fluids
            RFMeshofstream.write((char *) &mesh_vel_data.Z, sizeof(float));
            RFMeshofstream.write((char *) &mesh_vel_data.Y, sizeof(float));
            RFMeshofstream.write((char *) &mesh_vel_data.X, sizeof(float));

#ifdef DEBUG
            std::cout << "mesh_vel_data = " << mesh_vel_data.X << "\t"
                      << mesh_vel_data.Y << "\t" << mesh_vel_data.Z << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshVelocityData(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : writeMeshFileEOF()
   *
   *  Description : Write the Real Flow mesh file end of file marker
   *
   *  This method returns 1 if there's a file error
   *
   *  Input Arguments : None
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   inline int RealFlow_Mesh_File::writeMeshFileEOF()
   {
      unsigned int eof_code = 0xDEDEDEDE;

#ifdef DEBUG
      std::cout << std::endl << "RealFlow_Mesh_File::writeMeshFileEOF() - Writing mesh file end of file marker" << std::endl << std::endl;
#endif

      try {
            // Write the number of fluids
            RFMeshofstream.write((char *) &eof_code, sizeof(unsigned int));
         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::writeMeshFileEOF(): EXCEPTION: " << e.what() << std::endl;
            RFMeshofstream.clear();
            RFMeshofstream.close();
            return 1;
         }

      return 0;

   }



   /* ******************************************************************************
   *  Function Name : closeMeshFile()
   *
   *  Description :  Close the Real Flow mesh file
   *
   *
   *  Input Arguments : int mode
   *
   *  Return Value : int
   *
   ***************************************************************************** */
   int RealFlow_Mesh_File::closeMeshFile(int mode)
   {

#ifdef DEBUG
      std::cout << "Closing Real Flow mesh file" << std::endl << std::endl;
#endif

      try {

            if(mode == RF_FILE_READ) {
                  RFMeshifstream.close();
               }
            else {
                  RFMeshofstream.close();
               }
#ifdef DEBUG
            std::cout << "RealFlow_Mesh_File::closeMeshFile(): Closed Real Flow mesh input stream" << std::endl;
#endif

         }
      catch(std::ios_base::failure & e) {
            std::cerr << "RealFlow_Mesh_File::closeMeshFile() - EXCEPTION: " << e.what() << std::endl;
            RFMeshifstream.clear();
            return 1;
         }


      return 0;

   }


}


#endif



/**********************************************************************************/
//  $Log: real_flow_mesh.C,v $
//  Revision 1.2  2011-02-11 04:18:01  mstory
//  Modifications for most of the H11 changes.  Still need to modify the attribute getters and setters ...
//
//
//  --mstory
//
//  Revision 1.1.1.1  2009-01-30 02:24:05  mstory
//  Initial inport of the Real Flow plugin source to the new DCA cvs reporitory.
//
//
//
/**********************************************************************************/



