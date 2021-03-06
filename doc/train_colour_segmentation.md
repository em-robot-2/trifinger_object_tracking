# How to Train the Color Segmentation

The color segmentation is done using a xgboost model which is trained in Python
and then converted to static C++ code.

To retrain the model, use the following steps:

First, train the model:

    rosrun trifinger_object_tracking train_xgb_tree.py --train \
        --image-dir $(rospack find trifinger_object_tracking)/data/cube_image_set_p4_2020-09-16

This creates the following files in the current working directory:

- data.pkl: The data set generated from the images.
- xgb_model.bin: The trained model in binary format
- xgb_model.bin_dump.txt: A text file with a dump of the model

The text dump generated above contains the actual feature names, which is nice
in general but unfortunately the "dump to C++" conversion will not work with
this.  Therefore we need to create a dump without the feature names:

    import xgboost
    model = xgboost.XGBClassifier()
    model.load_model("xgb_model.bin")
    model.get_booster().dump_model("xgb_model_dump.txt")

You can also use `scripts/get_xgb_dump.py` which does exactly that.  The dump
produced this way does not know about the original feature names but just calls
them "f0", "f1", ...
You may want to keep the original dump with the feature names as well, so you
can compare which feature "fx" corresponds to.

From this second dump, we can now generate the C++ code.  This
can be easily converted to a C++ file with static if/else statements using
[xgb2cpp](https://github.com/popcorn/xgb2cpp):

    python generate_cpp_code.py --num_classes 7 --xgb_dump xgb_model.bin_dump.txt

This creates a file `xgboost_classifier.cpp` in the current working directory.
Copy this file to `src/` of the trifinger_object_tracking package.  Then open
it and apply the following changes:

1. Adjust the header include to

       #include <trifinger_object_tracking/xgboost_classifier.h>

2. Replace `std::vector` with `std::array` (for better performance):

       std::array<float, XGB_NUM_CLASSES> xgb_classify(std::array<float, XGB_NUM_FEATURES> &sample) {

         std::array<float, XGB_NUM_CLASSES> sum;
         sum.fill(0.0);
