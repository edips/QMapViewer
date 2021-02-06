// Edit feature function, it opens feature form to edit, save or delete the feature
function editFeature(pair) {
    // If the feature geometry isPairValid and Toggle editing is checked, open the feature panel
    if (digitizing.isPairValid(pair)) {
        // Add the feature
        featurePanel.show_panel(pair, "Add", "form")
        console.log("Feature Panel must open")
    } else {
        console.log("Feature Panel didn't open")
        showMessage("Editing feature is not valid")
    }
}

// For Polygon
// Area item
function areaItem() {
    featureMenu.addItem( area.createObject( featureMenu, { text: "Area" } ) )
}

// Length item
function lengthItem() {
    featureMenu.addItem( length.createObject( featureMenu, { text: "Length" } ) )
}

function editAttributeItem() {
    featureMenu.addItem(edit_attribute.createObject(featureMenu, { text: "Edit Attributes" }))
}

function areaUnits( area, area_metric ) {
    if (areacombo.currentIndex === 0){
        area.text = ( area_metric ).toFixed( 2 )
    }
    // km2
    else if( areacombo.currentIndex === 1 ){
        area.text = ( area_metric * 0.000001 ).toFixed( 2 )
    }
    // ha
    else if( areacombo.currentIndex === 2 ){
        area.text = ( area_metric * 0.0001 ).toFixed( 2 )
    }
    // acre
    else if( areacombo.currentIndex === 3 ){
        area.text = ( area_metric * 0.000247105381467165 ).toFixed( 2 )
    }
    // mile
    else if( areacombo.currentIndex === 4 ){
        area.text = ( area_metric * 0.000000386102158542446 ).toFixed( 2 )
    }
    // yard
    else if( areacombo.currentIndex === 5 ){
        area.text = ( area_metric * 1.19599004630108).toFixed( 2 )
    }
    // feet
    else if( areacombo.currentIndex === 6 ){
        area.text = ( ( area_metric * 10.7639104167097 ) ).toFixed( 2 )
    }
}

function lengthUnits( length, length_metric ) {
    // meter
    if ( lengthcombo.currentIndex === 0 ){
        length.text = ( length_metric ).toFixed( 2 )
    }
    // kilometer
    else if( lengthcombo.currentIndex === 1 ){
        length.text = ( length_metric * 0.001 ).toFixed( 2 )
    }
    // miles
    else if( lengthcombo.currentIndex === 2 ){
        length.text = ( length_metric * 0.0006213712 ).toFixed( 2 )
    }
    // n. miles
    else if( lengthcombo.currentIndex === 3 ){
        length.text = ( length_metric * 0.0005399568 ).toFixed( 2 )
    }
    // yard 1.09361
    else if( lengthcombo.currentIndex === 4 ){
        length.text = ( length_metric * 1.0936132983 ).toFixed( 2 )
    }
    // feet
    else if( lengthcombo.currentIndex === 5 ){
        length.text = ( length_metric * 3.280839895 ).toFixed( 2 )
    }
}

// Scale bar units
function scalebar_settings(){
    if(__appSettings.scaleUnit === 0){
        return QgsQuick.QgsUnitTypes.MetricSystem
    }else{
        return QgsQuick.QgsUnitTypes.ImperialSystem
    }
}
