package EndPoints

import (
	"AllSecure/TeamServer/Common/Builder"
	"AllSecure/TeamServer/Common/Types"
	"errors"
)

func CreateImplant(ImplantData *Types.ImplantConfig, ProjectDir string) error {

	builder := Builder.NewImplantBuilder(ImplantData, ProjectDir)
	success := builder.Build()
	if !success {
		return errors.New("failed to build implant")
	}

	return nil
}
