
MPEG-7 General Sound Recognition and Indexing: Model Training and Testing Tools
Copyright 2001 Mitsubishi Electric Research Laboratories, CRL  All Rights Reserved.

      Permission to use, copy, modify and distribute this software and
      its documentation for educational, research and non-profit
      purposes, without fee, and without a written agreement is hereby
      granted, provided that the above copyright notice and the
      following three paragraphs appear in all copies.

      To request Permission to incorporate this software into
      commercial products contact MERL - Mitsubishi Electric Research
      Laboratories, 201 Broadway, Cambridge, MA 02139.

      IN NO EVENT SHALL MERL BE LIABLE TO ANY PARTY FOR DIRECT,
      INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
      INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
      AND ITS DOCUMENTATION, EVEN IF MERL HAS BEEN ADVISED OF THE
      POSSIBILITY OF SUCH DAMAGES.

      MERL SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
      FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED
      HEREUNDER IS ON AN "AS IS" BASIS, AND MERL HAS NO OBLIGATIONS TO
      PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
      MODIFICATIONS.

      Author:
      
	Michael Casey PhD   | M E R L  http://www.merl.com/people/casey
	Research Scientist  | Mitsubishi Electric Research Laboratories
	617.621.7535fax7550 | 201 Broadway 8th floor Cambridge MA 02139


Training a Classifier
=====================

To train a new classifier:

1) Create class sub-directory in the main TrainingData directory:
   e.g., for a 3-way classifier:

>cd TrainingData
>mkdir Laughter
>ls TrainingData

TrainingData/Male/
TrainingData/Female/
TrainingData/Laughter/

2) Copy about 100 sound files representing each class into the
   directory. The sound files MUST be 16kHz MONO WAV files. (You can
   convert your existing files to the correct format using
   CoolEdit2000 from www.syntrillium.com).

The Male and Female directories have been pre-defined with examples
from the DARPA TIMIT database. You may wish to extend these training
sets to include languages other English, or you may create a new class
for each language.

3) Start matlab in the MP7AudioIndexing directory and type the
   command:


>>Nstates=10; Ncomps=10; TrainSoundModels('TrainingData',Nstates,Ncomps,'myFirstClassifier');

After a few minutes of training, the system will write the models to a
file and go into TEST mode. In Test mode a number of examples from the
training data are used to test the classifier performance. These
examples were not actually used in training, they were held back for
cross-validation purposes. The system randomly splits the WAV files
into a 70% training and 30% testing set. If you want to do this
manually you must edit the individual xxx_TestList.txt and
xxx_TrainList.txt files in each of the class sub-directories.

After testing the results will be displayed in a table and written to
a file.

