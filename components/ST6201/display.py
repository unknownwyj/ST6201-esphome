import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display, spi, power_supply
from esphome.const import (
    CONF_BACKLIGHT_PIN,
    CONF_DC_PIN,
    CONF_HEIGHT,
    CONF_ID,
    CONF_LAMBDA,
    CONF_MODEL,
    CONF_RESET_PIN,
    CONF_WIDTH,
    CONF_POWER_SUPPLY,
    CONF_ROTATION,
    CONF_CS_PIN,
    CONF_OFFSET_HEIGHT,
    CONF_OFFSET_WIDTH,
)
from . import ST6201_ns

CONF_EIGHTBITCOLOR = "eightbitcolor"

CODEOWNERS = ["@kbx81"]

DEPENDENCIES = ["spi"]

ST6201 = ST6201_ns.class_(
    "ST6201", cg.PollingComponent, spi.SPIDevice, display.DisplayBuffer
)

MODEL_PRESETS = "model_presets"
REQUIRE_PS = "require_ps"


def model_spec(require_ps=False, presets=None):
    if presets is None:
        presets = {}
    return {MODEL_PRESETS: presets, REQUIRE_PS: require_ps}


MODELS = {
    "TEST_DISPLAY": model_spec(
        presets={
            CONF_HEIGHT: 480,
            CONF_WIDTH: 272,
            CONF_OFFSET_HEIGHT: 0,
            CONF_OFFSET_WIDTH: 0,
            CONF_CS_PIN: "GPIO10",
            CONF_DC_PIN: "GPIO8",
            CONF_RESET_PIN: "GPIO13",
            CONF_BACKLIGHT_PIN: "GPIO3",
        }
    ),
    "CUSTOM": model_spec(),
}


def validate_ST6201(config):
    model_data = MODELS[config[CONF_MODEL]]
    presets = model_data[MODEL_PRESETS]
    for key, value in presets.items():
        if key not in config:
            if key.endswith("pin"):
                # All pins are output.
                value = pins.gpio_output_pin_schema(value)
            config[key] = value

    if model_data[REQUIRE_PS] and CONF_POWER_SUPPLY not in config:
        raise cv.Invalid(
            f'{CONF_POWER_SUPPLY} must be specified when {CONF_MODEL} is {config[CONF_MODEL]}"'
        )

    if (
        CONF_OFFSET_WIDTH not in config
        or CONF_OFFSET_HEIGHT not in config
        or CONF_HEIGHT not in config
        or CONF_WIDTH not in config
    ):
        raise cv.Invalid(
            f"{CONF_HEIGHT}, {CONF_WIDTH}, {CONF_OFFSET_HEIGHT} and {CONF_OFFSET_WIDTH} must all be specified"
        )
    if CONF_DC_PIN not in config or CONF_RESET_PIN not in config:
        raise cv.Invalid(f"both {CONF_DC_PIN} and {CONF_RESET_PIN} must be specified")

    return config


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ST6201),
            cv.Required(CONF_MODEL): cv.one_of(*MODELS.keys(), upper=True, space="_"),
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_BACKLIGHT_PIN): cv.Any(
                cv.boolean,
                pins.gpio_output_pin_schema,
            ),
            cv.Optional(CONF_POWER_SUPPLY): cv.use_id(power_supply.PowerSupply),
            cv.Optional(CONF_EIGHTBITCOLOR, default=False): cv.boolean,
            cv.Optional(CONF_HEIGHT): cv.int_,
            cv.Optional(CONF_WIDTH): cv.int_,
            cv.Optional(CONF_OFFSET_HEIGHT): cv.int_,
            cv.Optional(CONF_OFFSET_WIDTH): cv.int_,
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(spi.spi_device_schema(cs_pin_required=False)),
    validate_ST6201,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    await spi.register_spi_device(var, config)

    cg.add(var.set_model_str(config[CONF_MODEL]))

    cg.add(var.set_height(config[CONF_HEIGHT]))
    cg.add(var.set_width(config[CONF_WIDTH]))
    cg.add(var.set_offset_height(config[CONF_OFFSET_HEIGHT]))
    cg.add(var.set_offset_width(config[CONF_OFFSET_WIDTH]))

    cg.add(var.set_eightbitcolor(config[CONF_EIGHTBITCOLOR]))

    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))

    reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset))

    if CONF_BACKLIGHT_PIN in config and config[CONF_BACKLIGHT_PIN]:
        bl = await cg.gpio_pin_expression(config[CONF_BACKLIGHT_PIN])
        cg.add(var.set_backlight_pin(bl))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_POWER_SUPPLY in config:
        ps = await cg.get_variable(config[CONF_POWER_SUPPLY])
        cg.add(var.set_power_supply(ps))